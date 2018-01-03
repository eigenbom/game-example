#ifndef rendersystem_hpp
#define rendersystem_hpp

#include "entity.h"
#include "system.h"
#include "termbox.h"
#include "util.h"

enum class RenderLayer {
  Ground,
  GroundCover,
  Particles,
  MobBelow,
  Mob,
  MobAbove,
};

class Sprite: public Component {
public:
  vec2i position {0, 0};
  RenderLayer renderLayer { RenderLayer::Ground };
  std::string frames;
  uint16_t fg = TB_WHITE;
  uint16_t bg = TB_BLACK;
  bool animated = false;
  int frame = 0;
  int frameRate = 1;
  int frameCounter = 0;
  
  Sprite() = default;
  Sprite(std::string frames, bool animated, int frameRate, uint16_t fg, uint16_t bg, vec2i position, RenderLayer layer):frames(frames), position(position), animated(animated), frameRate(frameRate), fg(fg), bg(bg), renderLayer(layer) {
    if (animated){
      frame = randInt(0, 1);
      frameCounter = randInt(0, frameRate);
    }
  }
};

class Game;
class RenderSystem: public System {
public:
  RenderSystem(Game& game):game_(game){}
  void update() final;
  void handleEvent(const EvAny&) final {}

  void render();
protected:
  Game& game_;
};

#endif /* rendersystem_hpp */
