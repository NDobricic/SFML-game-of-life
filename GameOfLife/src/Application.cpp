#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

class GameOfLife
{
private:
    sf::RenderTexture* currentGen;
    sf::RenderTexture* nextGen;

    sf::VertexArray canvas;

    sf::Shader shader;

    sf::RenderStates states;

    void SwapTextures()
    {
        sf::RenderTexture* temp = currentGen;
        currentGen = nextGen;
        nextGen = temp;
    }

public:
    GameOfLife(unsigned int width, unsigned int height)
    {
        currentGen = new sf::RenderTexture();
        currentGen->create(width, height);
        currentGen->setRepeated(true);
        currentGen->display();

        nextGen = new sf::RenderTexture();
        nextGen->create(width, height);
        nextGen->setRepeated(true);
        nextGen->display();

        canvas.setPrimitiveType(sf::Quads);
        canvas.resize(4);
        canvas[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Color::White, sf::Vector2f(0, 0));
        canvas[1] = sf::Vertex(sf::Vector2f(0, height), sf::Color::White, sf::Vector2f(0, height));
        canvas[2] = sf::Vertex(sf::Vector2f(width, height), sf::Color::White, sf::Vector2f(width, height));
        canvas[3] = sf::Vertex(sf::Vector2f(width, 0), sf::Color::White, sf::Vector2f(width, 0));

        shader.loadFromFile("res/shaders/gameoflife.frag", sf::Shader::Fragment);
        shader.setUniform("resX", (float)width);
        shader.setUniform("resY", (float)height);

        states.shader = &shader;
    }

    const sf::Texture& GetTexture()
    {
        return currentGen->getTexture();
    }

    void SetImage(const sf::Image& image)
    {
        sf::Texture* startTexture = new sf::Texture;
        startTexture->loadFromImage(image);

        currentGen->draw(canvas, startTexture);
        currentGen->display();

        states.texture = &currentGen->getTexture();

        delete startTexture;
    }

    void Update()
    {
        states.texture = &currentGen->getTexture();

        nextGen->draw(canvas, states);
        nextGen->display();

        SwapTextures();
    }

    ~GameOfLife()
    {
        delete currentGen;
        delete nextGen;
    }
};

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const unsigned int FPS_LIMIT = 10;

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "John Conway's Game of Life");
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(FPS_LIMIT);

    GameOfLife game(WIDTH, HEIGHT);

    sf::Sprite* canvas = new sf::Sprite();

    sf::RenderStates states;

    sf::Image* initialState = new sf::Image();
    initialState->create(WIDTH, HEIGHT);
    for(unsigned int y = 0; y < HEIGHT; y++)
        for (unsigned int x = 0; x < WIDTH; x++)
        {
            unsigned char color = (rand() % 2) * 255;
            initialState->setPixel(x, y, sf::Color(color, color, color, 255));
        }

    game.SetImage(*initialState);

    delete initialState;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        game.Update();

        window.draw(sf::Sprite(game.GetTexture()));
        window.display();
    }

    return EXIT_SUCCESS;
}