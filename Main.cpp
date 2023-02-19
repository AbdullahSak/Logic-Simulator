#include <iostream>
#include <SFML/Graphics.hpp>
#include "Classes.h"

using namespace std;

int main()
{
	int i = 0;
	// create the window
	RenderWindow window(VideoMode(1000, 600), "Logic Simulator");
	Simulator Sim(window);

	// create objects for toolbar

	Sim.createAndGate(1);
	Sim.createOrGate(1);
	Sim.createNotGate(1);
	Sim.createExorGate(1);
	Sim.createDff(1);
	Sim.createClk(1);
	Sim.createVdd(1);
	Sim.createGnd(1);
	Sim.createLed(1);

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				// window closed
			case Event::Closed:
				window.close();
				break;

				// mouse pressed
			case Event::MouseButtonPressed:
				if (event.mouseButton.button == Mouse::Left)
				{
					cout << "x:" << event.mouseButton.x << endl;
					cout << "y:" << event.mouseButton.y << endl;
					//toolbar elements's coordinates
					if ((23 <= event.mouseButton.x && event.mouseButton.x <= 88) && (28 <= event.mouseButton.y && event.mouseButton.y <= 75))
					{
						cout << "And Gate is clicked" << endl;
						Sim.createAndGate(0);
					}
					else if ((41 <= event.mouseButton.x && event.mouseButton.x <= 73) && (93 <= event.mouseButton.y && event.mouseButton.y <= 127))
					{
						cout << "Or Gate is clicked" << endl;
						Sim.createOrGate(0);
					}
					else if ((39 <= event.mouseButton.x && event.mouseButton.x <= 68) && (153 <= event.mouseButton.y && event.mouseButton.y <= 176))
					{
						cout << "Not Gate is clicked" << endl;
						Sim.createNotGate(0);
					}
					else if ((44 <= event.mouseButton.x && event.mouseButton.x <= 74) && (203 <= event.mouseButton.y && event.mouseButton.y <= 237))
					{
						cout << "Exor Gate is clicked" << endl;
						Sim.createExorGate(0);
					}
					else if ((41 <= event.mouseButton.x && event.mouseButton.x <= 76) && (262 <= event.mouseButton.y && event.mouseButton.y <= 308))
					{
						cout << "D-Flip Flop is clicked" << endl;
						Sim.createDff(0);
					}
					else if ((37 <= event.mouseButton.x && event.mouseButton.x <= 70) && (334 <= event.mouseButton.y && event.mouseButton.y <= 364))
					{
						cout << "Clock is clicked" << endl;
						Sim.createClk(0);
					}
					else if ((40 <= event.mouseButton.x && event.mouseButton.x <= 78) && (391 <= event.mouseButton.y && event.mouseButton.y <= 422))
					{
						cout << "Voltage Source is clicked" << endl;
						Sim.createVdd(0);
					}
					else if ((47 <= event.mouseButton.x && event.mouseButton.x <= 69) && (449 <= event.mouseButton.y && event.mouseButton.y <= 471))
					{
						cout << "Ground is clicked" << endl;
						Sim.createGnd(0);
					}
					else if ((53 <= event.mouseButton.x && event.mouseButton.x <= 71) && (493 <= event.mouseButton.y && event.mouseButton.y <= 530))
					{
						cout << "Led is clicked" << endl;
						Sim.createLed(0);
					}
					else if ((150 <= event.mouseButton.x && event.mouseButton.x <= 310) && (15 <= event.mouseButton.y && event.mouseButton.y <= 40))
					{
						cout << "Start is clicked" << endl;
						while (true)
						{
							window.pollEvent(event);
							if ((event.type == Event::MouseButtonPressed) && (event.mouseButton.button == Mouse::Left))
							{
								if ((390 <= event.mouseButton.x && event.mouseButton.x <= 550) && (15 <= event.mouseButton.y && event.mouseButton.y <= 40))
								{
									cout << "Stop is clicked" << endl;
									break;
								}
							}
							Clock clock;
							Time time = clock.getElapsedTime();
							while (time.asMilliseconds() <= 500)
							{
								time = clock.getElapsedTime();
							}
							Sim.toggleClk();
							Sim.startSimulation();

							Sim.clearDrawDisplay();

						}
					}
					else if ((390 <= event.mouseButton.x && event.mouseButton.x <= 550) && (15 <= event.mouseButton.y && event.mouseButton.y <= 40))
					{
						cout << "Stop is clicked" << endl;

					}
					else if (Sim.isThereAnyGate(Vector2f(Mouse::getPosition(window))))
					{
						Sim.clickExistingObject(Sim.isThereAnyGate(Vector2f(Mouse::getPosition(window))));
					}
					else
						Sim.hideRedRectangle();
				}

				// mouse released
			case Event::MouseButtonReleased:
				i++;
				if (i == 2)
				{
					Sim.realeseObject();
					i = 0;
				}
				break;

			case Event::KeyPressed:
				if (event.key.code == Keyboard::Delete)
				{
					cout << "delete is pressed" << endl;
					Sim.deleteAllSelectedObjects();
					Sim.hideRedRectangle();
				}
				break;
			default:
				break;
			}
		}

		Sim.clearDrawDisplay();
	}

	return 0;
}