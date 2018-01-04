#include "rendersystem.h"

#include "game.h"

void RenderSystem::update(){
  const int slowBy = 2;
  static int slowDown = 0;
  bool updateAnimation = slowDown++ >= slowBy;
  if (updateAnimation) slowDown = 0;
  
  for (auto& sprite: game_.sprites.values()){
    if (updateAnimation){
      if (sprite.animated){
        sprite.frameCounter++;
        if (sprite.frameCounter >= sprite.frameRate && !sprite.frames.empty()){
          sprite.frame = (sprite.frame + 1) % sprite.frames.size();
          sprite.frameCounter = 0;
        }
      }
      
      if (sprite.flashTimer > 0){
        sprite.flashTimer--;
      }
    }
  }
}

void RenderSystem::render(){
  game_.window.clear();
  
  const vec2i ws { game_.window.width(), game_.window.height() };
  for (int y = 0; y < ws.y; y++){
    for(int x = 0; x < ws.x; x++){
      vec2i p = game_.worldCoord({x, y});
      if (game_.worldBounds.contains(p)){
        game_.window.set(x, y, game_.groundTile(p), TB_WHITE, TB_BLACK);
      }
    }
  }
  
  for (auto layer: { RenderLayer::Ground, RenderLayer::GroundCover, RenderLayer::Particles, RenderLayer::MobBelow, RenderLayer::Mob, RenderLayer::MobAbove}){
    for (const auto& sprite: game_.sprites.values()){
      if (sprite.renderLayer == layer){
        vec2i p = sprite.position;
        bool flash = sprite.flashTimer > 0;
        if (game_.worldBounds.contains(p)){
          vec2i sc = game_.screenCoord(p);
          game_.window.set(sc.x, sc.y, sprite.frames[sprite.frame], flash ? TB_WHITE : sprite.fg, sprite.bg);
        }
      }
    }
  }
}
