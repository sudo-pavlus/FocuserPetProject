#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Asset Loading");

    // 1. Load the Texture
    sf::Texture texture;

    // Use the relative path to your folder
    if (!texture.loadFromFile("assets/sprites/Test_girl/Idle.png"))
    {
        // If it fails, check if the "assets" folder is in the same 
        // directory as your .exe or project file
        std::cerr << "Failed to load Idle.png!" << std::endl;
    }


    // 2. Create the Sprite and link it to the texture
    sf::Sprite sprite(texture);

    // Optional: Position the sprite
    sprite.setPosition({ 200.f, 200.f });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        // 3. Draw the Sprite
        window.draw(sprite);

        window.display();
    }

    return 0;
}