#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({400, 300}), "SFML Works!");
    
    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition({150.f, 100.f});
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        window.draw(circle);
        window.display();
    }
    
    return 0;
}