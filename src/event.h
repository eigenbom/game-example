#ifndef event_hpp
#define event_hpp

#include "entity.h"
#include "mob.h"
#include "util.h"
#include "variant.h"

#include <sstream>
#include <string>

struct EvRemove   { ident entity; };
struct EvKillMob  { ident who; };
struct EvSpawnMob { MobType type; vec2i position; };
struct EvTryWalk  { ident mob; vec2i from; vec2i to; };
struct EvWalked   { ident mob; vec2i from; vec2i to; };

using  EvAny = variant<EvRemove, EvKillMob, EvSpawnMob, EvTryWalk, EvWalked>;

inline std::string to_string(const EvAny& any){
  std::ostringstream oss;
  if (any.is<EvRemove>()){
    auto& ev = any.get<EvRemove>();
    oss << "EvRemove {" << to_string(ev.entity) << "}";
  }
  else if (any.is<EvKillMob>()){
    auto& ev = any.get<EvKillMob>();
    oss << "EvKillMob {" << to_string(ev.who) << "}";
  }
  else if (any.is<EvSpawnMob>()){
    auto& ev = any.get<EvSpawnMob>();
    oss << "EvSpawnMob {" << to_string(ev.type) << ", " << to_string(ev.position) << "}";
  }
  else if (any.is<EvTryWalk>()){
    auto& ev = any.get<EvTryWalk>();
    oss << "EvTryWalk {" << to_string(ev.mob) << ", " << to_string(ev.from) << ", " << to_string(ev.to) << "}";
  }
  else if (any.is<EvWalked>()){
    auto& ev = any.get<EvWalked>();
    oss << "EvWalked {" << to_string(ev.mob) << ", " << to_string(ev.from) << ", " << to_string(ev.to) << "}";
  }
  return oss.str();
}

#endif
