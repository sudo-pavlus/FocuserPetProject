#include <iostream>
#include <optional>
#include <map>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <cmath>

// --- MASTER LINKER FIX (Keeps your project building) ---
#ifdef _DEBUG
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#else
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")
#endif
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "shell32.lib")
// -------------------------------------------------------

enum PetState { IDLE, WALK, DIOLOGUE, ATTACK, PROTECTION  };

struct PetAnimation {
    sf::Texture texture;
    int frameCount;      
    float frameDuration; 
    float stride;
};

static int GetTaskbarTop() {
    APPBARDATA appBarData = {};
	appBarData.cbSize = sizeof(appBarData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        return appBarData.rc.top;
    }

    return 0;
}

int main() {
    const int frameWidth = 128;
    const int frameHeight = 128-56;
	const float spriteScale = 2.5f;

	int taskbarTop = GetTaskbarTop();

	std::cout << "Taskbar top position: " << taskbarTop << std::endl;

    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(frameWidth * spriteScale),
        static_cast<unsigned int>(frameHeight * spriteScale) }), "Pet", sf::Style::None);
    window.setFramerateLimit(60);

    HWND hwnd = static_cast<HWND>(window.getNativeHandle());
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(255, 0, 255), 0, LWA_COLORKEY);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	window.setPosition({ 0, taskbarTop-static_cast<int>(frameHeight*spriteScale) });

    std::map<PetState, PetAnimation> animations;

    if (animations[IDLE].texture.loadFromFile("assets/sprites/Test_girl/Idle.png")) {
        animations[IDLE].frameCount = 7;
        animations[IDLE].frameDuration = 0.11666666666f;
        animations[IDLE].stride = 0.0f;
    }

    if (animations[WALK].texture.loadFromFile("assets/sprites/Test_girl/Walk.png")) {
        animations[WALK].frameCount = 12;
        animations[WALK].frameDuration = 0.08f;
        animations[WALK].stride = 10.0f;
    }

    if (animations[DIOLOGUE].texture.loadFromFile("assets/sprites/Test_girl/Dialogue.png")) {
        animations[DIOLOGUE].frameCount = 6;
        animations[DIOLOGUE].frameDuration = 0.1f;
        animations[DIOLOGUE].stride = 0.0f;
    }

    if (animations[ATTACK].texture.loadFromFile("assets/sprites/Test_girl/Attack.png")) {
        animations[ATTACK].frameCount = 9;
        animations[ATTACK].frameDuration = 0.1f;
        animations[ATTACK].stride = 0.0f;
    }

    if (animations[PROTECTION].texture.loadFromFile("assets/sprites/Test_girl/Protection.png")) {
        animations[PROTECTION].frameCount = 2;
        animations[PROTECTION].frameDuration = 0.1f;
        animations[PROTECTION].stride = 0.0f;
    }

    PetState currentState = IDLE;

    sf::Sprite sprite(animations[currentState].texture);
    sprite.setScale({ spriteScale, spriteScale });
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameWidth, frameHeight }));

    sf::Clock animationClock;
    int currentFrame = 0;

    sf::Clock movementClock;

    // Dragging and falling state
    bool isDragging = false;
    sf::Vector2i dragOffset = {0,0}; // global mouse - window pos at drag start
    bool isFalling = false;
    int fallTargetY = taskbarTop - static_cast<int>(frameHeight * spriteScale);
    const float fallSpeed = 1200.0f; // pixels per second for falling
    sf::Clock frameClock; // used for delta time during falling

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (event->is<sf::Event::KeyPressed>()) {
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
                    window.close();
            }

            // Mouse pressed -> start drag if on sprite
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();
                if (mb->button == sf::Mouse::Button::Left) {
                    sf::Vector2i localMouse = sf::Mouse::getPosition(window);
                    if (sprite.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(localMouse.x), static_cast<float>(localMouse.y)))) {
                        isDragging = true;
                        // store offset between global mouse pos and window top-left
                        sf::Vector2i globalMouse = sf::Mouse::getPosition();
                        sf::Vector2i winPos = window.getPosition();
                        dragOffset = globalMouse - winPos;
                        isFalling = false; // cancel falling while dragging
                        // set to idle state
                        if (currentState != IDLE) {
                            currentState = IDLE;
                            sprite.setTexture(animations[IDLE].texture);
                            currentFrame = 0;
                            animationClock.restart();
                        }
                        movementClock.restart();
                    }
                }
            }

            // Mouse released -> stop dragging and start falling
            if (event->is<sf::Event::MouseButtonReleased>()) {
                auto mb = event->getIf<sf::Event::MouseButtonReleased>();
                if (mb->button == sf::Mouse::Button::Left) {
                    if (isDragging) {
                        isDragging = false;
                        isFalling = true;
                        fallTargetY = taskbarTop - static_cast<int>(frameHeight * spriteScale);
                        frameClock.restart();
                        movementClock.restart();
                    }
                }
            }
        }

        PetAnimation& currentAnim = animations[currentState];

		sf::Vector2i mousePosLocal = sf::Mouse::getPosition(window);
        
		bool isCurserOn = sprite.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePosLocal.x), static_cast<float>(mousePosLocal.y)));

        // While left button held and dragging, move window to follow global mouse minus initial offset
        if (isDragging && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i globalMouse = sf::Mouse::getPosition();
            sf::Vector2i newWinPos = globalMouse - dragOffset;
            window.setPosition(newWinPos);
            // reset movementClock so autonomous motions wait until after drag
            movementClock.restart();
        }

        // If not dragging, handle hover protection and autonomous state changes
        if (!isDragging) {
            if (isCurserOn && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (currentState != PROTECTION) {
                    currentState = PROTECTION;
                    currentFrame = 0;
                    sprite.setTexture(animations[PROTECTION].texture);
                    sprite.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
                    animationClock.restart();
                }
                movementClock.restart();
            }
            else {
                if (currentState == PROTECTION) {
                    currentState = IDLE;
                    currentFrame = 0;
                    sprite.setTexture(animations[IDLE].texture);
                    sprite.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
                    animationClock.restart();
                    movementClock.restart();
                }
                if (movementClock.getElapsedTime().asSeconds() > 3.0f) {
                    if (currentState == IDLE) {
                        currentState = WALK;
                        currentFrame = 0;
                    }
                    else if (currentState == WALK) {
                        currentState = DIOLOGUE;
                        sf::Vector2i pos = window.getPosition();
                        pos.x += static_cast<int>(11 * spriteScale);
                        currentFrame = 0;
                    }
                    else {
                        currentState = IDLE;
                        currentFrame = 0;
                    }
                    sprite.setTexture(animations[currentState].texture);
                    sprite.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
                    animationClock.restart();
                    movementClock.restart();
                }
            }
        }

        // Falling animation (move the whole window vertically toward the taskbar)
        if (isFalling && !isDragging) {
            float dt = frameClock.restart().asSeconds();
            sf::Vector2i winPos = window.getPosition();
            int currentY = winPos.y;
            int dy = fallTargetY - currentY;
            if (std::abs(dy) <= static_cast<int>(fallSpeed * dt)) {
                winPos.y = fallTargetY;
                isFalling = false;
            } else {
                int move = static_cast<int>(std::copysign(1.0f, static_cast<float>(dy)) * fallSpeed * dt);
                winPos.y += move;
            }
            window.setPosition(winPos);
        }

        // Update animation frames
        if (animationClock.getElapsedTime().asSeconds() > currentAnim.frameDuration) {
            bool isProtection = (currentState == PROTECTION);
            bool isLastFrame = (currentFrame == currentAnim.frameCount - 1);

            if (!(isProtection && isLastFrame)) {
                currentFrame = (currentFrame + 1) % currentAnim.frameCount;
            }
            sprite.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));

            if (currentAnim.stride > 0 && !isDragging && !isFalling) {
                sf::Vector2i pos = window.getPosition();
                pos.x += static_cast<int>(currentAnim.stride);

                if (pos.x > 2560) pos.x = -static_cast<int>(frameWidth * spriteScale);

                window.setPosition(pos);
            }

            animationClock.restart();
        }
        
        // Draw
        window.clear(sf::Color::Magenta); 
        window.draw(sprite);
        window.display();
    }

    return 0;
}