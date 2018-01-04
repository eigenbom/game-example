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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

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

#ifdef __EMSCRIPTEN__
  EM_BOOL emsKeyDownCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
  void setSize(int width, int height);

private:
  std::vector<WindowEvent> eventsBuffer_;
  int width_ {60};
  int height_ {60};
#endif

private:
  std::vector<WindowEvent> events_;
};

#endif 