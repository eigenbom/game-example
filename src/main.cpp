
// A basic game structure example
// @eigenbom 2017

#include "game.h"
#include "mobsystem.h"
#include "rendersystem.h"
#include "system.h"
#include "util.h"
#include "window.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>

using namespace std::string_literals;

void Test(Game& game){
  const auto& b = game.worldBounds;
  game.groundTiles_.fill('.');
  
  int numMobs = 50;
  for (int i=0; i<numMobs; i++){
    MobType type = choose({MobType::Rabbit, MobType::Snake, MobType::OrcStrong});
    vec2i pos {
      randInt(b.left, b.left + b.width - 1),
      randInt(b.top - b.height + 1, b.top)
    };
    auto& mob = game.createMob(type, pos);
    game.queueEvent(EvKillMob { mob.id });
    
    if (i % 32 == 0) game.sync();
  }
  
  auto& player = game.createMob(MobType::Player, {0,0});
  game.player = player.entity;
  game.camera = player.position;
  
  game.sync();
}

void PopulateWorld(Game& game){
  const auto& b = game.worldBounds;
  
  game.groundTiles_.fill('.');
  for (int x = b.left; x < b.left + b.width; x++){
    for (int y = b.top - b.height + 1; y <= b.top; y++){
      if (randInt(0, 6) == 0){
        game.groundTile({x, y}) = choose({',','_',' '});
      }
    }
  }
  
  int numMobs = (int) (0.5 * sqrt(b.width * b.height));
  for (int i=0; i<numMobs; i++){
    MobType type = choose({MobType::Rabbit, MobType::OrcStrong}); //MobType::Snake, MobType::OrcStrong});
    vec2i pos {
      randInt(b.left, b.left + b.width - 1),
      randInt(b.top - b.height + 1, b.top)
    };
    game.createMob(type, pos);
    
    if (i % 32 == 0) game.sync();
  }
  auto& player = game.createMob(MobType::Player, {0,0});
  game.player = player.entity;
  game.camera = player.position;
  
  // Mob-less sprites
  for (int i=0; i<numMobs / 2; i++){
    vec2i pos {
      randInt(b.left, b.left + b.width - 1),
      randInt(b.top - b.height + 1, b.top)
    };
    if (randInt(0, 2) != 0){
      game.createSprite("vV", true, 6, TB_MAGENTA, TB_BLACK, pos, RenderLayer::GroundCover);
    }
    else if (randInt(0, 1) == 0){
      game.createSprite("|/-\\", true, 2, TB_YELLOW, TB_BLACK, pos, RenderLayer::GroundCover);
    }
    else {
      game.createSprite("Xx", true, 1, TB_BLUE, TB_BLACK, pos, RenderLayer::GroundCover);
    }
    
    if (i % 32 == 0) game.sync();
  }
  
  game.sync();
}

void runTest(){
  std::unique_ptr<Window> window {new Window};
  std::unique_ptr<Game>   game {new Game {*window}};
  Test(*game);
  while (window->handleEvents()){
    if (!game->update()) break;
    
    // randomly spawn or kill things
    if (randInt(0, 1) == 0){
      game->createMob(MobType::OrcWeak, vec2i {randInt(-20, 20), randInt(-20, 20)});
    }
    else {
      auto& vals = game->mobs.values();
      if (!vals.empty()){
        int ri = randInt(0, (int) vals.size() - 1);
        game->queueEvent(EvKillMob {vals[ri].id} );
      }
    }
  }
}

void runGame(){
  std::unique_ptr<Window> window {new Window};
  std::unique_ptr<Game>   game {new Game {*window}};
  
  PopulateWorld(*game);
  while (window->handleEvents()){
    if (!game->update()) break;
    game->render();
    window->render();
  }
}

int main(int argc, const char * argv[]) {
  // runTest();
  runGame();
  return 0;
}
