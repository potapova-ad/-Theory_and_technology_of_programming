#pragma once
#include "animation.hpp"
#include "player.hpp"

class Player;

class PlayerState
{
public:
  PlayerState();
  void setSprite(sf::Sprite &sprite, bool isFacedRight);

  virtual void handleEvents(Player *player, const sf::Event &event) = 0;
  virtual void update(Player *player, float dt);
  virtual void hook(Player *player) = 0;
  virtual void attacked(Player *player) = 0;
  virtual void startFalling(Player *player) = 0;
  virtual void hitGround(Player *player) = 0;
  virtual ~PlayerState();

  void jump(Player *player, float jumpingVelocity);

protected:
  Animation mAnimation;
  static constexpr float kJumpingVelocity = 1500;
  static constexpr float kSubJumpingVelocity = 1000;
};

class Idle final : public PlayerState
{
public:
  Idle(Player *player);

  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);

private:
};

class Running final : public PlayerState
{
public:
  Running(Player *player);
  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);

private:
  float mRunningSpeed;
};

class Sliding final : public PlayerState
{

public:
  Sliding(Player *player);
  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);

private:
  float mCurrentTime;
  static constexpr float kSlidingTime = 0.50;
  static constexpr float kVelocityMultiplier = 2.5;
  static constexpr float kVelocityDecay = 0.99;
};

class Falling final : public PlayerState
{
public:
  Falling(Player *player);
  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);

private:
  unsigned jumpCount = 0;
  static constexpr float kHorizontalVelocity = 800;
};

class Hooked final : public PlayerState
{
public:
  static constexpr float kMaxHookOffset = 15;

  Hooked(Player *player);
  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);
};

class Sitting final : public PlayerState
{
public:
  Sitting(Player *player);
  void update(Player *player, float dt);
  void handleEvents(Player *player, const sf::Event &event);
  void hook(Player *player);
  void attacked(Player *player);
  void startFalling(Player *player);
  void hitGround(Player *player);

private:
};
