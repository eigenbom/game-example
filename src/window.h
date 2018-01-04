#ifndef window_hpp
#define window_hpp

#include "termbox.h"
#include "util.h"

enum class WindowEvent {
  Unknown,
  ArrowUp,
  ArrowDown,
  ArrowLeft,
  ArrowRight,
};

inline std::string to_string(WindowEvent ev){
  switch (ev){
    case WindowEvent::Unknown: default: return "Unknown";
      
    case WindowEvent::ArrowUp:    return "ArrowUp";
    case WindowEvent::ArrowDown:  return "ArrowDown";
    case WindowEvent::ArrowLeft:  return "ArrowLeft";
    case WindowEvent::ArrowRight: return "ArrowRight";
  }
}

#ifdef NO_WINDOW

#include <iostream>
#include <thread>

class Window {
public:
  Window(){}
  ~Window(){}
  constexpr int32_t width() const  { return 256; }
  constexpr int32_t height() const { return 128; }
  
  bool handleEvents() {
    events_.clear();
    static int i = 0;
    static const std::vector<WindowEvent> evs {WindowEvent::ArrowUp,  WindowEvent::ArrowLeft, WindowEvent::ArrowDown, WindowEvent::ArrowRight};
    events_.push_back(evs[(i++)%4]);
    return true;
  }
  
  const std::vector<WindowEvent>& events() const { return events_; }
  
  void render() {
    std::cout << "Window::render()\n";
    using namespace std::chrono_literals;
    const auto delayPerFrame = 15ms;
    if (delayPerFrame != 0ms){
      std::this_thread::sleep_for(delayPerFrame);
    }
  }
  
  void clear() {}
  void set(int x, int y, char c, uint16_t fg, uint16_t bg) {}
protected:
  std::vector<WindowEvent> events_;
};

#else

class Window {
public:
  Window();
  ~Window();
  int32_t width() const;
  int32_t height() const;
  
  bool handleEvents(); // returns false on quit
  const std::vector<WindowEvent>& events() const { return events_; }
  void render();
  
  void clear();
  void set(int x, int y, char c, uint16_t fg, uint16_t bg);
protected:
  std::vector<WindowEvent> events_;
};

#endif

#endif /* window_hpp */
