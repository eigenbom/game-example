#ifndef game_hpp
#define game_hpp

#include "entity.h"
#include "event.h"
#include "mob.h"
#include "mobsystem.h"
#include "physics.h"
#include "physicssystem.h"
#include "rendersystem.h"
#include "system.h"
#include "util.h"
#include "window.h"

#include <deque>
#include <memory>
#include <string>
#include <utility>

class Game {
public:
  Game(Window& window);
  bool update();
  void render();
  
  recti worldBounds {-32, 13, 64, 26};
  ident player {invalid_id};
  vec2i camera {0, 0};
  
  bool cameraShake = false;
  int cameraShakeTimer = 0;
  vec2i cameraShakeOffset {0, 0};
  
  // Map between coordinate-spaces
  vec2i worldCoord(vec2i screenCoord) const {
    const vec2i ws { window.width(), window.height() };
    vec2i q = screenCoord - ws / 2;
    vec2i camFinal = camera + (cameraShake ? cameraShakeOffset : vec2i {0, 0});
    return { q.x + camFinal.x, -(q.y - camFinal.y) };
  }
  
  vec2i screenCoord(vec2i worldCoord) const {
    const vec2i ws { window.width(), window.height() };
    vec2i wc = worldCoord;
    vec2i camFinal = camera + (cameraShake ? cameraShakeOffset : vec2i {0, 0});
    return vec2i {wc.x - camFinal.x, camFinal.y - wc.y} + ws / 2;
  }
  
  buffered_container<Entity>  entities;
  buffered_container<Mob>     mobs;
  buffered_container<Sprite>  sprites;
  buffered_container<Physics> physics;
  
  Array2D<char> groundTiles_;
  char& groundTile(vec2i p){
    vec2i q {p.x - worldBounds.left, worldBounds.top - p.y};
    return groundTiles_(q);
  }
  
  Window& window;
  
  MobSystem mobSystem;
  PhysicsSystem physicsSystem;
  RenderSystem renderSystem;
  std::vector<System*> systems {
    &mobSystem,
    &physicsSystem,
    &renderSystem,
  };
  
  void queueEvent(const EvAny& ev);
  void sync();
  
  // Factories
  Sprite& createSprite(std::string frames, bool animated, int frameRate, uint16_t fg, uint16_t bg, vec2i position, RenderLayer layer);
  Mob& createMob(MobType type, vec2i position);
  void createBloodSplatter(vec2i position);
  
protected:
  int tick_ = 0;
  
  std::array<std::vector<EvAny>, 2> events_ {};
  int eventsIndex_ = 0;

  std::deque<std::pair<std::string, int>> eventLog_;
};

#endif
