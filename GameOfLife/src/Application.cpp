#include <SFML/Graphics.hpp>
#include <random>

/*----------------------------------------------------------------------/
SIMULATION CONTROLS:
- The simulation begins as soon as you start the application
- To pause/unpause the simulation use SPACE on your keyboard
- You can control the zoom using the scroll-wheel on your mouse
- You can drag the screen by holding the left mouse button
- Use ESCAPE on your keyboard to close the application
/----------------------------------------------------------------------*/

/*-------SIMULATION OPTIONS-------*/
const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

const unsigned int FPS_LIMIT = 0;
const bool ENABLE_VSYNC = true;

const bool FULLSCREEN = true;
/*--------------------------------*/

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
        shader.setUniform("resolution", sf::Vector2f(width, height));

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

int clamp(int number, int min, int max)
{
    if (number < min)
        return min;

    if (number > max)
        return max;

    return number;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "John Conway's Game of Life", (FULLSCREEN == true) ? sf::Style::Fullscreen : sf::Style::Close);
    window.setVerticalSyncEnabled(ENABLE_VSYNC);
    window.setFramerateLimit(FPS_LIMIT);

    GameOfLife game(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Sprite* canvas = new sf::Sprite();

    sf::RenderStates states;

    sf::Image* initialState = new sf::Image();
    initialState->create(WINDOW_WIDTH, WINDOW_HEIGHT);
    for(unsigned int y = 0; y < WINDOW_HEIGHT; y++)
        for (unsigned int x = 0; x < WINDOW_WIDTH; x++)
        {
            unsigned char color = (rand() % 2) * 255;
            initialState->setPixel(x, y, sf::Color(color, color, color, 255));
        }

    game.SetImage(*initialState);

    delete initialState;

    int cameraZoom = 0;
    sf::Vector2f cameraOffset(0, 0);

    sf::Shader camShader;
    camShader.loadFromFile("res/shaders/camera.frag", sf::Shader::Fragment);

    sf::Vector2f dragStart(0, 0);
    bool dragging = false;

    bool paused = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();

            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.delta > 0)
                    cameraZoom += 1;
                else
                    cameraZoom -= 1;

                cameraZoom = clamp(cameraZoom, 0, 5);

                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    dragStart = (sf::Vector2f)sf::Mouse::getPosition();
                    dragging = true;
                }

                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    dragging = false;
                }

                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                else if (event.key.code == sf::Keyboard::Space)
                    paused = !paused;

                break;

            default:
                break;
            }
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (dragging)
        {
            sf::Vector2f offset = dragStart - (sf::Vector2f)sf::Mouse::getPosition();

            offset.x /= WINDOW_WIDTH;
            offset.y /= WINDOW_HEIGHT;

            offset.y *= -1;

            cameraOffset += offset / (float)(1 << cameraZoom);

            dragStart = (sf::Vector2f)sf::Mouse::getPosition();
        }

        camShader.setUniform("offset", cameraOffset);
        camShader.setUniform("zoom", cameraZoom);

        window.clear();

        if (!paused)
            game.Update();

        window.draw(sf::Sprite(game.GetTexture()), &camShader);
        window.display();
    }

    return EXIT_SUCCESS;
}