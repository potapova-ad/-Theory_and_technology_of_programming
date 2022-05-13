#include "sfline.hpp"

class Node
{
public:
  enum class State
  {
    Blocked = 0,
    Unblocked,
    Activated
  };
  enum class MouseState
  {
    LeftButton = 0,
    RightButton,
    ErrorButton
  };

  Node(sf::Vector2f &position);
  sf::Vector2f getPosition();

  void onMousePressed(sf::Vector2f mouseCoords, MouseState state);
  void addChild(const std::shared_ptr<Node> &child);

  void block();
  void unblock();

  virtual void leftMouseButtonPressed(sf::Vector2f mouseCoords);
  virtual void rightMouseButtonPressed(sf::Vector2f mouseCoords);
  virtual bool collisionTest(sf::Vector2f mouseCoords) const = 0;
  virtual void draw(sf::RenderWindow &window) const = 0;
  virtual size_t getNodeStatus() const = 0;

  std::vector<std::shared_ptr<Node>> mChildren{};

protected:
  sf::Vector2f mPosition{0, 0};
  State mState = State::Blocked;

  inline static sf::Color sBlockedColor{40, 40, 40};
  inline static sf::Color sUnlockedColor{80, 80, 40};
  inline static sf::Color sActivatedColor{160, 160, 40};
};

Node::Node(sf::Vector2f &position) : mPosition{position} {}

void Node::addChild(const std::shared_ptr<Node> &child)
{
  mChildren.push_back(child);
}

sf::Vector2f Node::getPosition() { return mPosition; }

void Node::unblock() { mState = State::Unblocked; }

void Node::block()
{
  mState = State::Blocked;
  for (const auto &child : mChildren)
    child->block();
}

void Node::onMousePressed(sf::Vector2f mouseCoords, Node::MouseState state)
{
  switch (state)
  {
  case Node::MouseState::LeftButton:
    leftMouseButtonPressed(mouseCoords);
    break;
  case Node::MouseState::RightButton:
    rightMouseButtonPressed(mouseCoords);
    break;
  default:
    break;
  }
}

void Node::leftMouseButtonPressed(sf::Vector2f mouseCoords)
{
  if (mState == State::Blocked)
    return;

  if (collisionTest(mouseCoords))
  {
    if (mState == State::Unblocked)
    {
      mState = State::Activated;
      for (const auto &child : mChildren)
        child->unblock();
    }

    else if (mState == State::Activated)
    {
      mState = State::Unblocked;
      for (const auto &child : mChildren)
        child->block();
    }
  }

  for (const auto &child : mChildren)
  {
    child->leftMouseButtonPressed(mouseCoords);
  }
}

void Node::rightMouseButtonPressed(sf::Vector2f mouseCoords)
{
  if (mState == State::Blocked)
    return;

  if (collisionTest(mouseCoords))
  {
    if (mState == State::Activated)
    {
      mState = State::Unblocked;
      for (const auto &child : mChildren)
        child->block();
    }
  }

  for (const auto &child : mChildren)
  {
    child->rightMouseButtonPressed(mouseCoords);
  }
}

class AccumulateNode : public Node
{
public:
  size_t maxLevel = 0;
  size_t currentLevel = 0;
  sf::Text subTitle;
  sf::Texture mTexture;
  sf::Sprite mSprite;

  AccumulateNode(sf::Vector2f &position, const sf::Font &font,
                 size_t max_level);

  void leftMouseButtonPressed(sf::Vector2f mouseCoords) override;
  void rightMouseButtonPressed(sf::Vector2f mouseCoords) override;
  bool collisionTest(sf::Vector2f mouseCoords) const override;
  void draw(sf::RenderWindow &window) const override;
  size_t getNodeStatus() const override;

  virtual sf::String getIconPath() const = 0;

  void loadTexture();
  sf::Color getCurrentColor() const;

  inline static const size_t kCharacterSize = 16;
  inline static const size_t width = 64;
  inline static const size_t height = 80;
  inline static const size_t subtitle_offset = 20;
};

class SwordRectSkillNode : public AccumulateNode
{
public:
  inline static const size_t mxlvl = 6;

  SwordRectSkillNode(sf::Vector2f position, const sf::Font &font)
      : AccumulateNode{position, font, mxlvl}
  {
    loadTexture();
  }

  sf::String getIconPath() const override
  {
    return sf::String{"icons/icon_rect_sword.png"};
  }
};

class FreezeRectSkillNode : public AccumulateNode
{
public:
  inline static const size_t mxlvl = 6;

  FreezeRectSkillNode(sf::Vector2f position, const sf::Font &font)
      : AccumulateNode{position, font, mxlvl}
  {
    loadTexture();
  }

  sf::String getIconPath() const override
  {
    return sf::String{"icons/icon_rect_freeze.png"};
  }
};

class ChainRectSkillNode : public AccumulateNode
{
public:
  inline static const size_t mxlvl = 6;

  ChainRectSkillNode(sf::Vector2f position, const sf::Font &font)
      : AccumulateNode{position, font, mxlvl}
  {
    loadTexture();
  }

  sf::String getIconPath() const override
  {
    return sf::String{"icons/icon_rect_chain.png"};
  }
};

std::shared_ptr<Node> anotherTree(const sf::Font &font);

AccumulateNode::AccumulateNode(sf::Vector2f &position, const sf::Font &font,
                               size_t max_level)
    : Node(position)
{
  std::string subtitle = std::to_string(0) + "/" + std::to_string(max_level);
  std::string maxsize =
      std::to_string(max_level) + "/" + std::to_string(max_level);

  subTitle.setFont(font);
  subTitle.setCharacterSize(kCharacterSize);
  subTitle.setFillColor(sBlockedColor);
  subTitle.setString(subtitle);
  subTitle.setOrigin(
      {(float)maxsize.length() / 2, (float)maxsize.length() / 2});
  subTitle.setPosition({position.x, position.y + subtitle_offset});

  currentLevel = 0;
  maxLevel = max_level;
}

void AccumulateNode::leftMouseButtonPressed(sf::Vector2f mouseCoords)
{
  if (mState == State::Blocked)
    return;

  if (collisionTest(mouseCoords))
  {
    if (mState == State::Unblocked)
    {
      mState = State::Activated;
      currentLevel = 1;
      subTitle.setString(std::to_string(currentLevel) + "/" +
                         std::to_string(maxLevel));
      for (const auto &child : mChildren)
        child->unblock();
    }
    else if (mState == State::Activated)
    {
      currentLevel = (currentLevel == maxLevel) ? maxLevel : currentLevel + 1;
      subTitle.setString(std::to_string(currentLevel) + "/" +
                         std::to_string(maxLevel));
    }
  }
  for (const auto &child : mChildren)
    child->leftMouseButtonPressed(mouseCoords);
}

void AccumulateNode::rightMouseButtonPressed(sf::Vector2f mouseCoords)
{
  if (mState == State::Blocked)
    return;

  if (collisionTest(mouseCoords))
  {
    if (mState == State::Activated)
    {
      if (currentLevel == 1)
      {
        mState = State::Unblocked;
        currentLevel = 0;
        for (const auto &child : mChildren)
          child->block();
      }
      else
        --currentLevel;

      subTitle.setString(std::to_string(currentLevel) + "/" +
                         std::to_string(maxLevel));
    }
  }
  if (mState == State::Unblocked)
    subTitle.setString(std::to_string(0) + "/" + std::to_string(maxLevel));

  for (const auto &child : mChildren)
    child->rightMouseButtonPressed(mouseCoords);
}

void AccumulateNode::loadTexture()
{
  sf::String texturePath = getIconPath();
  if (!mTexture.loadFromFile(texturePath))
  {
    std::cout << "Error! Can't load file " << texturePath.toAnsiString()
              << std::endl;
    std::exit(1);
  }
  mSprite.setTexture(mTexture);
  mSprite.setOrigin({width / 2, height / 2});
  mSprite.setPosition(mPosition);
}

bool AccumulateNode::collisionTest(sf::Vector2f mouseCoords) const
{
  sf::Vector2f d = mPosition - mouseCoords;
  return (std::abs(d.x) < width) && (std::abs(d.y) < height);
}

sf::Color AccumulateNode::getCurrentColor() const
{
  if (mState == State::Unblocked)
    return sUnlockedColor;
  else if (mState == State::Activated)
    return sActivatedColor;
  return sBlockedColor;
}

void AccumulateNode::draw(sf::RenderWindow &window) const
{
  for (const auto &el : mChildren)
  {
    sfLine connectionLine{mPosition, el->getPosition(), getCurrentColor(), 2};
    connectionLine.draw(window);
    el->draw(window);
  }

  static sf::RectangleShape shape({width, height});
  shape.setOrigin({width / 2, height / 2});
  shape.setFillColor(getCurrentColor());
  shape.setPosition(mPosition);
  window.draw(shape);

  window.draw(mSprite);
  window.draw(subTitle);
}

size_t AccumulateNode::getNodeStatus() const
{
  size_t sum = 0;
  for (auto &child : mChildren)
  {
    sum += child->getNodeStatus();
  }
  return sum + currentLevel;
}

std::shared_ptr<Node> anotherTree(const sf::Font &font)
{
  std::shared_ptr<Node> root{new SwordRectSkillNode({400, 500}, font)};
  root->addChild(
      std::shared_ptr<Node>{new SwordRectSkillNode({200, 400}, font)});
  root->addChild(
      std::shared_ptr<Node>{new FreezeRectSkillNode({400, 400}, font)});
  root->addChild(
      std::shared_ptr<Node>{new ChainRectSkillNode({600, 400}, font)});
  return root;
}

class HitNode : public Node
{
public:
  HitNode(sf::Vector2f position);
  sf::Color getCurrentColor() const;

  void loadTexture();
  void draw(sf::RenderWindow &window) const;

  bool collisionTest(sf::Vector2f mouseCoords) const override;
  size_t getNodeStatus() const override;

  virtual sf::String getIconPath() = 0;

private:
  sf::Texture mTexture;
  sf::Sprite mSprite;

  float mRadius = 24;
  bool mIsActivated = false;
};

std::shared_ptr<Node> createSkillTree();

class BombSkillNode : public HitNode
{
public:
  BombSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_bomb.png"};
  }
};

class SpikesSkillNode : public HitNode
{
public:
  SpikesSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_spikes.png"};
  }
};

class LightningSkillNode : public HitNode
{
public:
  LightningSkillNode(sf::Vector2f position) : HitNode{position}
  {
    loadTexture();
  }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_lightning.png"};
  }
};

class EyeSkillNode : public HitNode
{
public:
  EyeSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_eye.png"};
  }
};

class ClawsSkillNode : public HitNode
{
public:
  ClawsSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_claws.png"};
  }
};

class ShieldSkillNode : public HitNode
{
public:
  ShieldSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_shield.png"};
  }
};

class SwordSkillNode : public HitNode
{
public:
  SwordSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_sword.png"};
  }
};

class ShurikenSkillNode : public HitNode
{
public:
  ShurikenSkillNode(sf::Vector2f position) : HitNode{position}
  {
    loadTexture();
  }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_shuriken.png"};
  }
};

class WindSkillNode : public HitNode
{
public:
  WindSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_shuriken.png"};
  }
};

class MeteoriteSkillNode : public HitNode
{
public:
  MeteoriteSkillNode(sf::Vector2f position) : HitNode{position}
  {
    loadTexture();
  }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_meteorite.png"};
  }
};

class HandSkillNode : public HitNode
{
public:
  HandSkillNode(sf::Vector2f position) : HitNode{position} { loadTexture(); }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_hand.png"};
  }
};

class EarthquakeSkillNode : public HitNode
{
public:
  EarthquakeSkillNode(sf::Vector2f position) : HitNode{position}
  {
    loadTexture();
  }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_earthquake.png"};
  }
};

class FireballSkillNode : public HitNode
{
public:
  FireballSkillNode(sf::Vector2f position) : HitNode(position)
  {
    loadTexture();
  }

  sf::String getIconPath() override
  {
    return sf::String{"icons/icon_fireball.png"};
  }
};

HitNode::HitNode(sf::Vector2f position) : Node{position} {}

void HitNode::loadTexture()
{
  sf::String texturePath = getIconPath();
  if (!mTexture.loadFromFile(texturePath))
  {
    std::cout << "Error! Can't load file " << texturePath.toAnsiString()
              << std::endl;
    std::exit(1);
  }
  mSprite.setTexture(mTexture);
  mSprite.setOrigin({mRadius, mRadius});
  mSprite.setPosition(mPosition);
}

sf::Color HitNode::getCurrentColor() const
{
  if (mState == State::Unblocked)
    return sUnlockedColor;
  else if (mState == State::Activated)
    return sActivatedColor;
  return sBlockedColor;
}

void HitNode::draw(sf::RenderWindow &window) const
{
  for (const auto &el : mChildren)
  {
    sfLine connectionLine{mPosition, el->getPosition(), getCurrentColor(), 2};
    connectionLine.draw(window);
    el->draw(window);
  }

  static sf::CircleShape shape(mRadius);
  shape.setOrigin({mRadius, mRadius});
  shape.setFillColor(getCurrentColor());
  shape.setPosition(mPosition);
  window.draw(shape);

  window.draw(mSprite);
}

bool HitNode::collisionTest(sf::Vector2f mouseCoords) const
{
  sf::Vector2f d = mPosition - mouseCoords;
  return d.x * d.x + d.y * d.y < mRadius * mRadius;
}

size_t HitNode::getNodeStatus() const
{
  size_t sum = 0;
  for (auto &child : mChildren)
  {
    sum += child->getNodeStatus();
  }
  return (mState == State::Activated) ? sum + 1 : sum;
}

std::shared_ptr<Node> createSkillTree()
{
  std::shared_ptr<Node> root{new LightningSkillNode({400, 500})};
  std::shared_ptr<Node> a{new ShurikenSkillNode({200, 400})};
  std::shared_ptr<Node> b{new BombSkillNode({400, 400})};
  std::shared_ptr<Node> c{new EyeSkillNode({600, 400})};
  root->addChild(a);
  root->addChild(b);
  root->addChild(c);

  a->addChild(std::shared_ptr<Node>{new ShieldSkillNode({100, 200})});
  a->addChild(std::shared_ptr<Node>{new SwordSkillNode({200, 200})});
  a->addChild(std::shared_ptr<Node>{new EarthquakeSkillNode({300, 200})});

  b->addChild(std::shared_ptr<Node>{new HandSkillNode({400, 200})});
  b->addChild(std::shared_ptr<Node>{new MeteoriteSkillNode({500, 200})});
  std::shared_ptr<Node> e{new BombSkillNode({600, 200})};
  b->addChild(e);
  e->addChild(std::shared_ptr<Node>{new WindSkillNode({500, 100})});
  e->addChild(std::shared_ptr<Node>{new SwordSkillNode({600, 100})});
  c->addChild(std::shared_ptr<Node>{new EyeSkillNode({700, 200})});
  std::shared_ptr<Node> f{new FireballSkillNode({100, 700})};
  a->addChild(f);

  return root;
}

class AbstructSkillTree
{
public:
  std::shared_ptr<Node> root;
  sf::Text Title;
  std::string Name;

  AbstructSkillTree(std::shared_ptr<Node> new_root, const sf::Font &font, const std::string title, size_t max_skill_points, sf::Color textColor);
  void onMousePressed(sf::Vector2f mouseCoord, Node::MouseState state);
  void draw(sf::RenderWindow &window) const;
  void addChild(const std::shared_ptr<Node> &child);

  static inline const sf::Vector2f title_offset{-25, 50};
  static inline const size_t kCharacterSize = 16;

  size_t currPoints = 0;
  size_t maxPoints;
};

class WarriorSkillTree : public AbstructSkillTree
{
public:
  WarriorSkillTree(sf::Vector2f pos, const sf::Font &font) : AbstructSkillTree(std::shared_ptr<Node>{new SwordRectSkillNode{pos, font}}, font, std::string("Warrior\n"), 10, sf::Color{255, 255, 255})
  {
    addChild(std::shared_ptr<Node>{new SwordSkillNode({pos.x, pos.y - 100})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new EarthquakeSkillNode({pos.x - 50, pos.y - 150})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new SpikesSkillNode({pos.x + 50, pos.y - 150})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new BombSkillNode({pos.x, pos.y - 200})});
    root->mChildren[0]->mChildren[2]->addChild(std::shared_ptr<Node>{new MeteoriteSkillNode({pos.x + 50, pos.y - 250})});
    root->mChildren[0]->mChildren[2]->addChild(std::shared_ptr<Node>{new ShieldSkillNode({pos.x - 50, pos.y - 250})});
    root->mChildren[0]->mChildren[2]->mChildren[0]->addChild(std::shared_ptr<Node>{new ClawsSkillNode({pos.x + 50, pos.y - 350})});
    root->mChildren[0]->mChildren[2]->mChildren[1]->addChild(std::shared_ptr<Node>{new WindSkillNode({pos.x - 50, pos.y - 350})});
    root->unblock();
  }
};

class RogueSkillTree : public AbstructSkillTree
{
public:
  RogueSkillTree(sf::Vector2f pos, const sf::Font &font) : AbstructSkillTree(std::shared_ptr<Node>{new ChainRectSkillNode{pos, font}}, font, std::string("Rogue\n"), 10, sf::Color{255, 255, 255})
  {
    addChild(std::shared_ptr<Node>{new HandSkillNode({pos.x, pos.y - 100})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new SwordSkillNode({pos.x - 50, pos.y - 170})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new WindSkillNode({pos.x + 50, pos.y - 170})});
    root->mChildren[0]->mChildren[0]->addChild(std::shared_ptr<Node>{new BombSkillNode({pos.x - 50, pos.y - 250})});
    root->mChildren[0]->mChildren[1]->addChild(std::shared_ptr<Node>{new SpikesSkillNode({pos.x + 50, pos.y - 250})});
    root->mChildren[0]->mChildren[1]->mChildren[0]->addChild(std::shared_ptr<Node>{new ClawsSkillNode({pos.x, pos.y - 350})});
    root->mChildren[0]->mChildren[1]->mChildren[0]->addChild(std::shared_ptr<Node>{new MeteoriteSkillNode({pos.x + 50, pos.y - 320})});
    root->mChildren[0]->mChildren[1]->mChildren[0]->addChild(std::shared_ptr<Node>{new EyeSkillNode({pos.x + 100, pos.y - 350})});
    root->mChildren[0]->mChildren[0]->mChildren[0]->addChild(std::shared_ptr<Node>{new EarthquakeSkillNode({pos.x - 50, pos.y - 320})});
    root->unblock();
  }
};

class MageSkillTree : public AbstructSkillTree
{
public:
  MageSkillTree(sf::Vector2f pos, const sf::Font &font) : AbstructSkillTree(std::shared_ptr<Node>{new FreezeRectSkillNode{pos, font}}, font, std::string("Mage\n"), 10, sf::Color{255, 255, 255})
  {
    addChild(std::shared_ptr<Node>{new EyeSkillNode({pos.x, pos.y - 100})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new LightningSkillNode({pos.x - 50, pos.y - 200})});
    root->mChildren[0]->addChild(std::shared_ptr<Node>{new WindSkillNode({pos.x + 50, pos.y - 200})});
    root->mChildren[0]->mChildren[0]->addChild(std::shared_ptr<Node>{new HandSkillNode({pos.x - 100, pos.y - 300})});
    root->mChildren[0]->mChildren[1]->addChild(std::shared_ptr<Node>{new MeteoriteSkillNode({pos.x + 50, pos.y - 350})});
    root->mChildren[0]->mChildren[0]->addChild(std::shared_ptr<Node>{new ClawsSkillNode({pos.x - 50, pos.y - 350})});
    root->mChildren[0]->mChildren[0]->addChild(std::shared_ptr<Node>{new EarthquakeSkillNode({pos.x, pos.y - 300})});
    root->unblock();
  }
};

AbstructSkillTree::AbstructSkillTree(std::shared_ptr<Node> new_root, const sf::Font &font, const std::string s_title, size_t max_skill_points, sf::Color textColor)
{
  maxPoints = max_skill_points;
  Name = s_title;
  currPoints = 0;

  std::string title = Name + std::to_string(currPoints) + "/" + std::to_string(maxPoints);
  Title.setFont(font);
  Title.setCharacterSize(kCharacterSize);
  Title.setFillColor(textColor);
  Title.setOrigin({(float)title.length(), 0});
  Title.setPosition(new_root->getPosition() + title_offset);
  Title.setString(title);

  root = new_root;
}

void AbstructSkillTree::onMousePressed(sf::Vector2f mouseCoord, Node::MouseState state)
{
  switch (state)
  {
  case Node::MouseState::LeftButton:
  {
    if (currPoints != maxPoints)
      root->leftMouseButtonPressed(mouseCoord);
    currPoints = root->getNodeStatus();
    Title.setString(Name + std::to_string(currPoints) + "/" + std::to_string(maxPoints));
    break;
  }
  case Node::MouseState::RightButton:
  {
    root->rightMouseButtonPressed(mouseCoord);
    currPoints = root->getNodeStatus();
    Title.setString(Name + std::to_string(currPoints) + "/" + std::to_string(maxPoints));
    break;
  }
  }
}

void AbstructSkillTree::draw(sf::RenderWindow &window) const
{
  root->draw(window);
  window.draw(Title);
}

void AbstructSkillTree::addChild(const std::shared_ptr<Node> &child)
{
  root->addChild(child);
}

int main()
{
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(800, 800), "Skill Tree",
                          sf::Style::Close, settings);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("consolas.ttf"))
  {
    std::cout << "Can't load font" << std::endl;
  }

  MageSkillTree mage_tree{{200, 500}, font};
  WarriorSkillTree war_tree{{400, 500}, font};
  RogueSkillTree rog_tree{{600, 500}, font};

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseButtonPressed)
      {
        sf::Vector2f mouseCoords =
            window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        if (event.mouseButton.button == sf::Mouse::Left)
        {
          mage_tree.onMousePressed(mouseCoords, Node::MouseState::LeftButton);
          war_tree.onMousePressed(mouseCoords, Node::MouseState::LeftButton);
          rog_tree.onMousePressed(mouseCoords, Node::MouseState::LeftButton);
        }
        if (event.mouseButton.button == sf::Mouse::Right)
        {
          mage_tree.onMousePressed(mouseCoords, Node::MouseState::RightButton);
          war_tree.onMousePressed(mouseCoords, Node::MouseState::RightButton);
          rog_tree.onMousePressed(mouseCoords, Node::MouseState::RightButton);
        }
      }
    }

    window.clear(sf::Color::Black);
    mage_tree.draw(window);
    war_tree.draw(window);
    rog_tree.draw(window);
    window.display();
  }

  return 0;
}