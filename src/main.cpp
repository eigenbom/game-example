// A basic game structure example
// @eigenbom 2017

#include "game.h"
#include "window.h"

#include <memory>

#ifndef __EMSCRIPTEN__ // Terminal Mode

void runGame(){
  std::unique_ptr<Window> window { new Window };
  std::unique_ptr<Game>   game { new Game {*window} };
  
  game->setup();
  while (window->handleEvents()){
    if (!game->update()) break;
    game->render();
    window->render();
  }
}

int main(int argc, const char * argv[]) {
  runGame();
  return 0;
}

#else // Emscripten Mode

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

Window* window {nullptr};
Game* game {nullptr};

EM_BOOL emsKeyDownCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData){
  return window->emsKeyDownCallback(eventType, e, userData);
}

extern "C" {
    void EMSCRIPTEN_KEEPALIVE _setWindowSize(int width, int height){
      if (window != nullptr){
        window->setSize(width, height);
      }
    }

    void EMSCRIPTEN_KEEPALIVE _setup()  { 
      emscripten_set_keydown_callback(nullptr, nullptr, true, emsKeyDownCallback);

      window = new Window;
      game = new Game{*window};
      game->setup();
    }

    void EMSCRIPTEN_KEEPALIVE _update() {
      window->handleEvents();
      game->update();
      game->render();
      window->render();
    }
}

#endif