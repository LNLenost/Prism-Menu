#include "../hacks.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#ifndef GEODE_IS_MACOS
#include <Geode/modify/CCDrawNode.hpp>
#endif

class $modify(PlayerObject) {
    bool isActuallyDart;
#ifndef GEODE_IS_MACOS // for whatever reason, fields arent found!
    // No Solids
    /*
     * +       bool collidedWithObject(float, GameObject*, cocos2d::CCRect, bool) = win 0x2cc450;
+       TodoReturn collidedWithObject(float, GameObject*) = win 0x2cc3d0;
    bool collidedWithObject(float p0, GameObject* obj, cocos2d::CCRect p2, bool p3) { // what is the point of not having p2, because this doesnt work without it
        if (Hacks::isHackEnabled("Enable Patching")) return PlayerObject::collidedWithObject(p0, obj, p2, p3);
        if (!Hacks::isHackEnabled("No Solids")) return PlayerObject::collidedWithObject(p0, obj, p2, p3);
        //return PlayerObject::collidedWithObject(p0, obj, p2, p3);
        return false;
    }*/
    bool was_platformer;
    /*bool collidedWithObject(float fl, GameObject* obj,  cocos2d::CCRect p0, bool p1) {
        return PlayerObject::collidedWithObject(fl, obj, p0, p1);
    }*/
    // Freeze Player
    void update(float dt) {
        if (!m_fields->was_platformer) {
            m_fields->was_platformer = this->m_isPlatformer;
        }
        if (Hacks::isHackEnabled("Force Platformer Mode")) {
            togglePlatformerMode(true);
        } else {
            togglePlatformerMode(m_fields->was_platformer);
        }
        auto gravityHack = Hacks::getHack("Gravity Value");
        if (Hacks::isHackEnabled("Change Gravity")) { // assume its enabled 
            m_gravityMod = gravityHack->value.floatValue;
        }
        if (Hacks::isHackEnabled("Instant Complete")) return;
        if (Hacks::isHackEnabled("Enable Patching") || !Hacks::isHackEnabled("Freeze Player")) return PlayerObject::update(dt);
    }
    bool init(int p0, int p1, GJBaseGameLayer *p2, cocos2d::CCLayer *p3, bool p4) {
        if (!PlayerObject::init(p0,p1,p2,p3,p4)) return false;
        return true;
    }

    void playerDestroyed(bool p0) {
        m_fields->was_platformer = this->m_isPlatformer;
        PlayerObject::playerDestroyed(p0);
    }
    #endif
    void pushButton(PlayerButton p0) {
        if (!Hacks::isHackEnabled("Enable Patching") && Hacks::isHackEnabled("Jump Hack")) PlayerObject::boostPlayer(10.0F); // idk if i should make this customizable
        PlayerObject::pushButton(p0);
    }
    void toggleDartMode(bool p0, bool p1) {
        // this is the fix until someone actually creates pads for android32 and android64, because i cant use m_isDart
        m_fields->isActuallyDart = p0;
        PlayerObject::toggleDartMode(p0, p1);
    }
    void activateStreak() { // i still need this because no one found m_isDart on mac
        /*if (!m_isDart && Hacks::isHackEnabled("No Trail")) return;
        if (m_isDart && Hacks::isHackEnabled("No Wave Trail")) return;*/
        if (!m_fields->isActuallyDart && Hacks::isHackEnabled("No Trail")) return;
        if (m_fields->isActuallyDart && Hacks::isHackEnabled("No Wave Trail")) return;
        PlayerObject::activateStreak();
    }
};

class $modify(GJBaseGameLayer) {
    #ifndef GEODE_IS_MACOS
    // No Mirror Transition, Instant Mirror Portal
    void toggleFlipped(bool p0, bool p1) { // i spent a lot of time figuring out why CCActionTween wont hook, only to realize that p1 instantly transitions it
        if (Hacks::isHackEnabled("Enable Patching")) return GJBaseGameLayer::toggleFlipped(p0, p1);
        if (!Hacks::isHackEnabled("No Mirror Transition")) GJBaseGameLayer::toggleFlipped(p0, Hacks::isHackEnabled("Instant Mirror Portal"));
    }
    #endif
};

// Solid Wave Trail
class $modify(CCDrawNode) {
	bool drawPolygon(CCPoint *p0, unsigned int p1, const ccColor4F &p2, float p3, const ccColor4F &p4) {
        if (!Hacks::isHackEnabled("Solid Wave Trail")) return CCDrawNode::drawPolygon(p0,p1,p2,p3,p4);
        if (p2.r == 1.F && p2.g == 1.F && p2.b == 1.F && p2.a != 1.F) return true; // tried doing just p2.a != 1.F but uh
        this->setBlendFunc(CCSprite::create()->getBlendFunc());
        this->setZOrder(-1); // ok but why
        return CCDrawNode::drawPolygon(p0,p1,p2,p3,p4);
    }
};
