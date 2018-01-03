#ifndef mobsystem_hpp
#define mobsystem_hpp

#include "mob.h"
#include "system.h"
#include "util.h"

#include <cstdint>
#include <string>

extern const std::unordered_map<MobType, MobInfo> MobDatabase;

class Game;
class MobSystem: public System {
public:
  MobSystem(Game& game):game_(game){}
  void update() final;
  void handleEvent(const EvAny&) final;
protected:
  Game& game_;
};

#endif /* mobsystem_hpp */
