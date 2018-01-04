#include "window.h"
#include "termbox.h"

#include <iostream>
#include <thread>

#ifdef NO_WINDOW // Null window (for testing)

#include <iostream>
#include <thread>

Window::Window(){}
Window::~Window(){}

int32_t Window::width() const  { return 256; }
int32_t Window::height() const { return 128; }
  
bool Window::handleEvents() {
  events_.clear();
  static int i = 0;
  static const std::vector<WindowEvent> evs {WindowEvent::ArrowUp,  WindowEvent::ArrowLeft, WindowEvent::ArrowDown, WindowEvent::ArrowRight};
  events_.push_back(evs[(i++)%4]);
  return true;
}
  
const std::vector<WindowEvent>& Window::events() const { return events_; }

void Window::render() {
  std::cout << "Window::render()\n";
  using namespace std::chrono_literals;
  const auto delayPerFrame = 15ms;
  if (delayPerFrame != 0ms){
    std::this_thread::sleep_for(delayPerFrame);
  }
}

void Window::clear() {}

void Window::set(int x, int y, char c, uint16_t fg, uint16_t bg) {}

#elif defined(__EMSCRIPTEN__) // Emscripten Window

#include <array>
#include <iostream>
#include <string>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

Window::Window(){
  
}

Window::~Window(){

}

bool Window::handleEvents(){
  events_.clear();
  events_ = eventsBuffer_;
  eventsBuffer_.clear();
  return true;
}

EM_BOOL Window::emsKeyDownCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
  using namespace std::string_literals;
  if      (e->key == "ArrowDown"s)  eventsBuffer_.push_back(WindowEvent::ArrowDown);
  else if (e->key == "ArrowUp"s)    eventsBuffer_.push_back(WindowEvent::ArrowUp);
  else if (e->key == "ArrowLeft"s)  eventsBuffer_.push_back(WindowEvent::ArrowLeft);
  else if (e->key == "ArrowRight"s) eventsBuffer_.push_back(WindowEvent::ArrowRight);
  return false;
}

void Window::setSize(int w, int h){
  width_  = w;
  height_ = h;
}

void Window::render(){
  EM_ASM(Module.flush());
}

void Window::clear(){
  EM_ASM(Module.clear());
}

void Window::set(int x, int y, char ch, uint16_t fg_, uint16_t bg_){
  using rgb = std::array<int, 3>;

  auto map_colour = [](uint16_t col) -> rgb {
    switch (col){
      default:
      case TB_DEFAULT: return {255, 255, 255};
      case TB_BLACK: return {0, 0, 0};
      case TB_RED: return {255, 0, 0};
      case TB_GREEN: return {0, 255, 0};
      case TB_YELLOW: return {255, 255, 0};
      case TB_BLUE: return {0, 0, 255};
      case TB_MAGENTA: return {255, 0, 255};
      case TB_CYAN: return {0, 255, 255};
      case TB_WHITE: return {255, 255, 255};
    }
  };

  rgb fg = map_colour(fg_);
  rgb bg = map_colour(bg_);

  int i = x;
  int j = y;
  EM_ASM({Module.set_char_and_colour($0, $1, String.fromCharCode($2), $3, $4, $5, $6, $7, $8)}, i, j, ch, fg[0], fg[1], fg[2], bg[0], bg[1], bg[2]);
}

int32_t Window::width() const {
  return width_;
}

int32_t Window::height() const {
  return height_;
}

#else // Terminal

Window::Window(){
  int code = tb_init();
  if (code < 0) {
    std::cerr << "termbox init failed, code: %d\n";
    throw std::runtime_error("termbox failed");
  }
  
  tb_select_input_mode(TB_INPUT_ESC);
  tb_clear();
  tb_select_output_mode(TB_OUTPUT_NORMAL);
}

Window::~Window(){
  tb_shutdown();
}

bool Window::handleEvents(){
  events_.clear();
  
  struct tb_event ev;
  if (tb_peek_event(&ev, 10)) {
    switch (ev.type) {
      case TB_EVENT_KEY:
        switch (ev.key) {
          case TB_KEY_ESC:
            return false;
            break;
          case TB_KEY_ARROW_LEFT:
            events_.push_back(WindowEvent::ArrowLeft);
            break;
          case TB_KEY_ARROW_RIGHT:
            events_.push_back(WindowEvent::ArrowRight);
            break;
          case TB_KEY_ARROW_UP:
            events_.push_back(WindowEvent::ArrowUp);
            break;
          case TB_KEY_ARROW_DOWN:
            events_.push_back(WindowEvent::ArrowDown);
            break;
        }
        break;
      case TB_EVENT_RESIZE:
        // draw_all();
        break;
    }
  }
  
  return true;
}

void Window::render(){
  using namespace std::chrono_literals;
  const auto delayPerFrame = 15ms;
  
  tb_present();
  
  if (delayPerFrame != 0ms){
    std::this_thread::sleep_for(delayPerFrame);
  }
}

void Window::clear(){
  tb_clear();
}

void Window::set(int x, int y, char c, uint16_t fg, uint16_t bg){
  tb_change_cell(x, y, c, fg, bg);
}

int32_t Window::width() const {
  return tb_width();
}

int32_t Window::height() const {
  return tb_height();
}

#endif
