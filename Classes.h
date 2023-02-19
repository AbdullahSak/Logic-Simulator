#include <SFML/Graphics.hpp>
#include <iostream>

#define MAX_CONNECTIONS 32

using namespace std;
using namespace sf;

class Object
{
protected:
	Object* next;
	bool locked;
	RenderWindow* window;
	Texture textures[2];
	Sprite sprite;
	bool state;
	bool selected;

public:
	friend class Simulator;
	Object(bool, RenderWindow&);
	virtual ~Object();
	virtual void Simulate() {}
};

Object::Object(bool locked, RenderWindow& param_window) : locked(locked)
{
	next = NULL;
	selected = false;
	window = &param_window;
	cout << "Object is created" << endl;
}

Object::~Object()
{
	cout << "Object is destructed" << endl;
}

class Pin
{
	enum pinType { INPUT, OUTPUT };
	enum pinState { HIGHZ, LOW, HIGH }; //enum for pin state
	int index;
	Pin* connectedTo[MAX_CONNECTIONS]; //List of other pins this pin is connected to
	Object* wires[MAX_CONNECTIONS]; //Wires connected to this pin
	int numConnections; //Number of connections on this pin
	Vector2f pos; //Screen position of the pin
	pinType type; //whether this pin is input or output
	pinState state; //Logic state of the signal on this pin
public:
	Pin();
	~Pin();
	void setPinType(int);
	bool isItInput();
	void setPinPosition(int, int, float, float);
	Vector2f getPinPosition();
	Object* getWire(int);
	int getConnectionNum();
	int getPinIndex();
	void setPinState(int);
	int getPinState();
	void addWireConnection(Object*);
	void deleteWireConnection(Object*);
	void addPinConnection(Pin*);
	void deletePinConnection(Pin*);
};

Pin::Pin()
{
	index = -1;
	state = HIGHZ;
	numConnections = 0;
	cout << "Pin is created with default constuctor" << endl;
}

Pin::~Pin()
{
	cout << "Pin is destructed" << endl;
}

void Pin::setPinType(int param_type)
{
	if (param_type == 0)
		type = INPUT;
	else if (param_type == 1)
		type = OUTPUT;
	else
		cout << "Invalid type" << endl;
}

bool Pin::isItInput()
{
	if (type == INPUT)
		return true;
	else
		return false;
}

void Pin::setPinPosition(int param_type, int param_index, float param_x, float param_y)
{
	setPinType(param_type);
	index = param_index;
	state = HIGHZ;
	pos.x = param_x;
	pos.y = param_y;
}

Vector2f Pin::getPinPosition()
{
	return pos;
}

Object* Pin::getWire(int index) {
	return wires[index];
}

int Pin::getConnectionNum()
{
	return numConnections;
}

int Pin::getPinIndex()
{
	return index;
}

void Pin::setPinState(int param_state)
{
	if (param_state == 0)
		state = LOW;
	else if (param_state == 1)
		state = HIGH;
	else if (param_state == -1)
		state = HIGHZ;
}

int Pin::getPinState()
{
	if (state == HIGH)
		return 1;
	else if (state == LOW)
		return 0;
	else
		return -1;
}

void Pin::addWireConnection(Object* wire)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (!wires[i])
		{
			wires[i] = wire;
			numConnections++;
			break;
		}
	}
}

void Pin::deleteWireConnection(Object* wire)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (wires[i])
		{
			if (wires[i] == wire)
			{
				wires[i] = NULL;
				numConnections--;
				cout << "Pin is disconnected to wire :" << i << endl;

				break;
			}
		}

	}
}

void Pin::addPinConnection(Pin* pin)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (!connectedTo[i])
		{
			connectedTo[i] = pin;
			break;
		}
	}
}

void Pin::deletePinConnection(Pin* pin)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (connectedTo[i])
		{
			if (connectedTo[i] == pin)
			{
				connectedTo[i] = NULL;
				break;
			}
		}
	}
}

class Wire : public Object
{
	Vertex line[2]; //End point vertices for the wire
	Pin* pins[2]; //A list of pins that this wire is connected to
public:
	Wire(Vertex, Pin*, bool, RenderWindow&);
	~Wire();
	Vertex* getVertex();
	void setVertex(Vertex vertex_line)
	{
		line[1] = vertex_line;
	}
	void setPin(int, Pin*);
	Pin* getPin(int pin_index);
	void Simulate();
};

Wire::Wire(Vertex line0, Pin* pin0, bool param_locked, RenderWindow& param_window) : Object(param_locked, param_window)
{
	line[0] = line0;
	this->setPin(0, pin0);
	cout << "Wire is created" << endl;
}

Wire::~Wire()
{
	if (pins[0])
	{
		pins[0]->deleteWireConnection(this);
		pins[0]->deletePinConnection(pins[1]);
	}

	if (pins[1])
	{
		pins[1]->deleteWireConnection(this);
		pins[1]->deletePinConnection(pins[0]);
	}
	cout << "Wire is destructed" << endl;
}

Vertex* Wire::getVertex()
{
	return line;
}

void Wire::setPin(int pin_index, Pin* param_pin)
{
	pins[pin_index] = param_pin;
	pins[pin_index]->addWireConnection(this);

	//if both of the pins are not null, connect them together
	if (pins[0] && pins[1])
	{
		pins[0]->addPinConnection(pins[1]);
		pins[1]->addPinConnection(pins[0]);
	}
}

Pin* Wire::getPin(int pin_index)
{
	return pins[pin_index];
}

void Wire::Simulate()
{
	if (pins[1]->isItInput())
		pins[1]->setPinState(pins[0]->getPinState());
	else
		pins[0]->setPinState(pins[1]->getPinState());
}

class LogicElement : public Object
{
protected:
	Pin pins[4]; //List of pins of the logic element
	int numPins; //Number of pins of the logic element
public:
	friend class Simulator;
	LogicElement(int, bool, RenderWindow&);
	~LogicElement();
	int isItPin(float, float);
	virtual void setElementPinsPositions();
	virtual void Simulate();
};

LogicElement::LogicElement(int param_numPins, bool param_locked, RenderWindow& param_window) : Object(param_locked, param_window)
{
	numPins = param_numPins;
	cout << "Logic Gate is created" << endl;
}

LogicElement::~LogicElement()
{
	cout << "Logic Gate is destructed" << endl;
}

int LogicElement::isItPin(float param_x, float param_y)
{
	for (int i = 0; i < numPins; i++)
	{
		float pinPos_x = pins[i].getPinPosition().x;
		float pinPos_y = pins[i].getPinPosition().y;
		if ((abs(param_x - pinPos_x) < 8) && (abs(param_y - pinPos_y) < 8))
		{
			cout << pins[i].getPinIndex() << "th pin is clicked." << endl;
			return pins[i].getPinIndex();
		}
	}
	return -1;
}

void LogicElement::setElementPinsPositions()
{

}

void LogicElement::Simulate()
{

}

class AndGate : public LogicElement
{
public:
	AndGate(bool param_locked, RenderWindow& param_window) : LogicElement(3, param_locked, param_window)
	{
		cout << "And gate is created" << endl;
	}
	~AndGate()
	{
		cout << "And gate is destructed" << endl;
	}
	void setElementPinsPositions();
	void Simulate();
};

void AndGate::setElementPinsPositions()
{
	Vector2f pos = sprite.getPosition();
	float and_x = pos.x;
	float and_y = pos.y;
	pins[0].setPinPosition(0, 0, and_x - float(42 * 0.75), and_y - float(15 * 0.75)); //set the first input pin
	pins[1].setPinPosition(0, 1, and_x - float(42 * 0.75), and_y + float(14 * 0.75)); //set the second input pin
	pins[2].setPinPosition(1, 2, and_x + float(44 * 0.75), and_y); //set the output pin
}

void AndGate::Simulate()
{
	//both inputs are high
	if (pins[0].getPinState() && pins[1].getPinState())
		pins[2].setPinState(1);
	//any of inputs is highz
	else if (pins[0].getPinState() == -1 || pins[1].getPinState() == -1)
		pins[2].setPinState(-1);
	else
		pins[2].setPinState(0);
}

class OrGate : public LogicElement
{
public:
	OrGate(bool param_locked, RenderWindow& param_window) : LogicElement(3, param_locked, param_window)
	{
		cout << "Or gate is created" << endl;
	}
	~OrGate()
	{
		cout << "Or gate is destructed" << endl;
	}
	void setElementPinsPositions();
	void Simulate();
};

void OrGate::setElementPinsPositions()
{
	float or_x = sprite.getPosition().x;
	float or_y = sprite.getPosition().y;
	pins[0].setPinPosition(0, 0, or_x - float(42 * 0.75), or_y - float(15 * 0.75)); //set the first input pin
	pins[1].setPinPosition(0, 1, or_x - float(42 * 0.75), or_y + float(14 * 0.75)); //set the second input pin
	pins[2].setPinPosition(1, 2, or_x + float(44 * 0.75), or_y); //set the output pin
}

void OrGate::Simulate()
{
	//any of inputs is highz
	if (pins[0].getPinState() == -1 || pins[1].getPinState() == -1)
		pins[2].setPinState(-1);
	//any of input is high
	else if (pins[0].getPinState() == 1 || pins[1].getPinState() == 1)
		pins[2].setPinState(1);
	else
		pins[2].setPinState(0);
}

class NotGate : public LogicElement
{
public:
	NotGate(bool param_locked, RenderWindow& param_window) : LogicElement(2, param_locked, param_window)
	{
		cout << "Not gate is created" << endl;
	}
	~NotGate()
	{
		cout << "Not gate is destructed" << endl;
	}
	void setElementPinsPositions();
	void Simulate();
};

void NotGate::setElementPinsPositions()
{
	float not_x = sprite.getPosition().x;
	float not_y = sprite.getPosition().y;
	pins[0].setPinPosition(0, 0, not_x - float(43 * 0.75), not_y); //set the input pin
	pins[1].setPinPosition(1, 1, not_x + float(43 * 0.75), not_y); //set the output pin
}

void NotGate::Simulate()
{
	if (pins[0].getPinState() == -1)
		pins[1].setPinState(-1);
	else if (pins[0].getPinState() == 1)
		pins[1].setPinState(0);
	else
		pins[1].setPinState(1);
}

class ExorGate : public LogicElement
{
public:
	ExorGate(bool param_locked, RenderWindow& param_window) : LogicElement(3, param_locked, param_window)
	{
		cout << "Exor gate is created" << endl;
	}
	~ExorGate()
	{
		cout << "Exor gate is destructed" << endl;
	}
	void setElementPinsPositions();
	void Simulate();
};

void ExorGate::setElementPinsPositions()
{
	float exor_x = sprite.getPosition().x;
	float exor_y = sprite.getPosition().y;
	pins[0].setPinPosition(0, 0, exor_x - float(42 * 0.75), exor_y - float(15 * 0.75)); //set the first input pin
	pins[1].setPinPosition(0, 1, exor_x - float(42 * 0.75), exor_y + float(14 * 0.75)); //set the second input pin
	pins[2].setPinPosition(1, 2, exor_x + float(44 * 0.75), exor_y); //set the output pin
}

void ExorGate::Simulate()
{
	//any of inputs is highz
	if (pins[0].getPinState() == -1 || pins[1].getPinState() == -1)
		pins[2].setPinState(-1);
	//any of inputs are different
	else if (pins[0].getPinState() != pins[1].getPinState())
		pins[2].setPinState(1);
	else
		pins[2].setPinState(0);
}

class Dff : public LogicElement
{
	int preState;
public:
	Dff(bool param_locked, RenderWindow& param_window) : LogicElement(4, param_locked, param_window)
	{
		cout << "D-Flip Flop is created" << endl;
		preState = pins[1].getPinState();

	}
	~Dff()
	{
		cout << "D-Flip Flop is destructed" << endl;
	}
	void setElementPinsPositions();
	void Simulate();
};

void Dff::setElementPinsPositions()
{
	float dff_x = sprite.getPosition().x;
	float dff_y = sprite.getPosition().y;
	pins[0].setPinPosition(0, 0, dff_x - float(44 * 0.75), dff_y - float(19 * 0.75)); //set the first input pin
	pins[1].setPinPosition(0, 1, dff_x - float(44 * 0.75), dff_y + float(2 * 0.75)); //set the second input pin
	pins[2].setPinPosition(1, 2, dff_x + float(44 * 0.75), dff_y - float(19 * 0.75)); //set the first output pin
	pins[3].setPinPosition(1, 3, dff_x + float(44 * 0.75), dff_y + float(21 * 0.75)); //set the second output pin
	//set initial state
	pins[2].setPinState(1);
	pins[3].setPinState(0);
}

void Dff::Simulate()
{
	if ((pins[1].getPinState() == 1) && (preState == 0) && (pins[0].getPinState() != -1))
	{
		pins[2].setPinState(pins[0].getPinState());
		pins[3].setPinState(!(pins[0].getPinState()));
	}
	preState = pins[1].getPinState();
}

class Led : public LogicElement
{
public:
	Led(bool param_locked, RenderWindow& param_window) : LogicElement(2, param_locked, param_window)
	{
		cout << "Led is created" << endl;
	}
	~Led()
	{
		cout << "Led is destructed" << endl;
	}
	void setElementPinsPositions();
	void ledOn();
	void ledOff();
	void Simulate();
};

void Led::setElementPinsPositions()
{
	float led_x = sprite.getPosition().x;
	float led_y = sprite.getPosition().y;
	pins[0].setPinPosition(0, 0, led_x - float(8 * 0.9), led_y + float(36 * 0.6)); //set the firt input pin
	pins[1].setPinPosition(0, 1, led_x + float(8 * 0.9), led_y + float(36 * 0.6)); //set the second input pin
}

void Led::ledOn()
{
	sprite.setTexture(textures[1]);
	sprite.setScale(Vector2f(0.8f, 0.6f));
}

void Led::ledOff()
{
	sprite.setTexture(textures[0]);
	sprite.setScale(Vector2f(0.8f, 0.6f));
}

void Led::Simulate()
{
	//any of inputs is highz
	if (pins[0].getPinState() == -1 || pins[1].getPinState() == -1)
		ledOff();
	//one of the inputs is high and the other is low
	else if (pins[0].getPinState() != pins[1].getPinState())
		ledOn();
	else
		ledOff();
}

class Clk : public LogicElement
{
public:
	Clk(bool param_locked, RenderWindow& param_window) : LogicElement(1, param_locked, param_window)
	{
		cout << "Clock is created" << endl;
	}
	~Clk()
	{
		cout << "Clock is destructed" << endl;
	}
	void setElementPinsPositions();
};

void Clk::setElementPinsPositions()
{
	float clk_x = sprite.getPosition().x;
	float clk_y = sprite.getPosition().y;
	pins[0].setPinPosition(1, 0, clk_x + float(37 * 0.75), clk_y); //set the output pin
	pins[0].setPinState(0);
}

class Vdd : public LogicElement
{
public:
	Vdd(bool param_locked, RenderWindow& param_window) : LogicElement(1, param_locked, param_window)
	{
		cout << "Voltage source is created" << endl;
	}
	~Vdd()
	{
		cout << "Voltage source is destructed" << endl;
	}
	void setElementPinsPositions();
};

void Vdd::setElementPinsPositions()
{
	float vdd_x = sprite.getPosition().x;
	float vdd_y = sprite.getPosition().y;
	pins[0].setPinPosition(1, 0, vdd_x, vdd_y + float(26 * 0.75)); //set the output pin
	pins[0].setPinState(1);
}

class Gnd : public LogicElement
{
public:
	Gnd(bool param_locked, RenderWindow& param_window) : LogicElement(1, param_locked, param_window)
	{
		cout << "Ground is created" << endl;
	}
	~Gnd()
	{
		cout << "Ground is destructed" << endl;
	}
	void setElementPinsPositions();
};

void Gnd::setElementPinsPositions()
{
	float gnd_x = sprite.getPosition().x;
	float gnd_y = sprite.getPosition().y;
	pins[0].setPinPosition(1, 0, gnd_x, gnd_y - float(14 * 0.75)); //set the output pin
	pins[0].setPinState(0);
}

class Simulator
{
	RenderWindow* window; //Pointer to SFML render window
	Object* objectsHead; //Pointer to a list of objects on screen

public:
	RectangleShape rectangle_horizontal;
	RectangleShape rectangle_vertical;
	RectangleShape red_rectangle;
	Texture textureStart;
	Sprite spriteStart;
	Texture textureStop;
	Sprite spriteStop;
	Texture textureAnd;
	Sprite spriteAnd;
	Texture textureOr;
	Sprite spriteOr;
	Texture textureNot;
	Sprite spriteNot;
	Texture textureExor;
	Sprite spriteExor;
	Texture textureDff;
	Sprite spriteDff;
	Texture textureLedOn;
	Sprite spriteLedOn;
	Texture textureLedOff;
	Sprite spriteLedOff;
	Texture textureClk;
	Sprite spriteClk;
	Texture textureVdd;
	Sprite spriteVdd;
	Texture textureGnd;
	Sprite spriteGnd;

	Simulator(RenderWindow&);


	void createAndGate(bool);
	void createOrGate(bool);
	void createNotGate(bool);
	void createExorGate(bool);
	void createDff(bool);
	void createClk(bool);
	void createVdd(bool);
	void createGnd(bool);
	void createLed(bool);
	void createWire(Vertex, Pin*, bool);
	void deleteObject();
	int getNumberOfSelection();
	void deleteAllSelectedObjects();
	void drawAllObjects();
	void realeseObject();
	void clickExistingObject(LogicElement*);
	void startSimulation();
	void simulateFromLogicElement(LogicElement*);
	LogicElement* isThereAnyGate(const Vector2f&);
	void showRectangleAroundLogicElement(LogicElement*);
	void hideRedRectangle();
	void toggleClk();
	void clearDrawDisplay();
	void selectConnectedWires(LogicElement*);

};

Simulator::Simulator(RenderWindow& param_window)
{
	objectsHead = NULL;
	window = &param_window;

	//set the toolbar background
	Color color(128, 128, 128);
	rectangle_horizontal.setSize(Vector2f(1000, 60));
	rectangle_horizontal.setFillColor(color);
	rectangle_horizontal.setPosition(0, 0);

	rectangle_vertical.setSize(Vector2f(120, 600));
	rectangle_vertical.setFillColor(color);
	rectangle_vertical.setPosition(0, 0);

	//set red rectangle

	red_rectangle.setSize(Vector2f(44.f, 66.f));
	red_rectangle.setFillColor(Color::Transparent);
	red_rectangle.setOutlineThickness(6);
	red_rectangle.setOutlineColor(Color::Red);
	red_rectangle.setOrigin(22.f, 33.f);
	red_rectangle.setPosition(2000.f, 2000.f);

	//load all textures

	textureStart.loadFromFile("../assets/START.png");

	textureStop.loadFromFile("../assets/STOP.png");

	textureAnd.loadFromFile("../assets/AND.png");

	textureOr.loadFromFile("../assets/OR.png");

	textureNot.loadFromFile("../assets/NOT.png");

	textureExor.loadFromFile("../assets/XOR.png");

	textureLedOn.loadFromFile("../assets/LEDON.png");

	textureLedOff.loadFromFile("../assets/LEDOFF.png");

	textureGnd.loadFromFile("../assets/GND.png");

	textureVdd.loadFromFile("../assets/VDD.png");

	textureClk.loadFromFile("../assets/CLOCK.png");

	textureDff.loadFromFile("../assets/DFF.png");

	// set all sprites

	spriteStart.setTexture(textureStart);

	spriteStop.setTexture(textureStop);

	spriteAnd.setTexture(textureAnd);
	spriteAnd.setOrigin(Vector2f(50.f, 30.f));

	spriteOr.setTexture(textureOr);
	spriteOr.setOrigin(Vector2f(50.f, 30.f));

	spriteNot.setTexture(textureNot);
	spriteNot.setOrigin(Vector2f(50.f, 30.f));

	spriteExor.setTexture(textureExor);
	spriteExor.setOrigin(Vector2f(50.f, 30.f));

	spriteDff.setTexture(textureDff);
	spriteDff.setOrigin(Vector2f(50.f, 40.f));

	spriteClk.setTexture(textureClk);
	spriteClk.setOrigin(Vector2f(40.f, 30.f));

	spriteVdd.setTexture(textureVdd);
	spriteVdd.setOrigin(Vector2f(30.f, 30.f));

	spriteGnd.setTexture(textureGnd);
	spriteGnd.setOrigin(Vector2f(23.f, 19.f));


	spriteLedOn.setTexture(textureLedOn);
	spriteLedOn.setOrigin(Vector2f(20.f, 39.f));

	spriteLedOff.setTexture(textureLedOff);
	spriteLedOff.setOrigin(Vector2f(20.f, 39.f));

	//place the start-stop buttons

	spriteStart.setPosition(Vector2f(150.f, 12.f));
	spriteStart.scale(Vector2f(0.75f, 0.75f));

	spriteStop.setPosition(Vector2f(390.f, 12.f));
	spriteStop.scale(Vector2f(0.75f, 0.75f));


}

void Simulator::createAndGate(bool param_locked)
{
	AndGate* AndObject = new AndGate(param_locked, *window);
	AndObject->textures[0] = textureAnd;
	AndObject->sprite = spriteAnd;
	Vector2f position = AndObject->sprite.getPosition();
	if (param_locked)
	{
		AndObject->sprite.setPosition(Vector2f(60.f, 55.f));
	}
	AndObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	AndObject->next = objectsHead;
	objectsHead = AndObject;
}

void Simulator::createOrGate(bool param_locked)
{
	OrGate* OrObject = new OrGate(param_locked, *window);
	OrObject->textures[0] = textureOr;
	OrObject->sprite = spriteOr;
	if (param_locked)
		OrObject->sprite.setPosition(Vector2f(60.f, 110.f));
	OrObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	OrObject->next = objectsHead;
	objectsHead = OrObject;
}

void Simulator::createNotGate(bool param_locked)
{
	NotGate* NotObject = new NotGate(param_locked, *window);
	NotObject->textures[0] = textureNot;
	NotObject->sprite = spriteNot;
	if (param_locked)
		NotObject->sprite.setPosition(Vector2f(60.f, 165.f));
	NotObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	NotObject->next = objectsHead;
	objectsHead = NotObject;
}

void Simulator::createExorGate(bool param_locked)
{
	ExorGate* ExorObject = new ExorGate(param_locked, *window);
	ExorObject->textures[0] = textureExor;
	ExorObject->sprite = spriteExor;
	if (param_locked)
		ExorObject->sprite.setPosition(Vector2f(60.f, 220.f));
	ExorObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	ExorObject->next = objectsHead;
	objectsHead = ExorObject;;
}

void Simulator::createDff(bool param_locked)
{
	Dff* DffObject = new Dff(param_locked, *window);
	DffObject->textures[0] = textureDff;
	DffObject->sprite = spriteDff;
	if (param_locked)
		DffObject->sprite.setPosition(Vector2f(60.f, 285.f));
	DffObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	DffObject->next = objectsHead;
	objectsHead = DffObject;
}

void Simulator::createClk(bool param_locked)
{
	Clk* ClkObject = new Clk(param_locked, *window);
	ClkObject->textures[0] = textureClk;
	ClkObject->sprite = spriteClk;
	if (param_locked)
		ClkObject->sprite.setPosition(Vector2f(60.f, 350.f));
	ClkObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	ClkObject->next = objectsHead;
	objectsHead = ClkObject;
}

void Simulator::createVdd(bool param_locked)
{
	Vdd* VddObject = new Vdd(param_locked, *window);
	VddObject->textures[0] = textureVdd;
	VddObject->sprite = spriteVdd;
	if (param_locked)
		VddObject->sprite.setPosition(Vector2f(60.f, 405.f));
	VddObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	VddObject->next = objectsHead;
	objectsHead = VddObject;
}

void Simulator::createGnd(bool param_locked)
{
	Gnd* GndObject = new Gnd(param_locked, *window);
	GndObject->textures[0] = textureGnd;
	GndObject->sprite = spriteGnd;
	if (param_locked)
		GndObject->sprite.setPosition(Vector2f(60.f, 460.f));
	GndObject->sprite.setScale(Vector2f(0.6f, 0.6f));
	GndObject->next = objectsHead;
	objectsHead = GndObject;
}

void Simulator::createLed(bool param_locked)
{
	Led* LedObject = new Led(param_locked, *window);
	LedObject->textures[0] = textureLedOff;
	LedObject->textures[1] = textureLedOn;
	LedObject->sprite = spriteLedOff;
	if (param_locked)
	{
		LedObject->sprite.setPosition(Vector2f(60.f, 515.f));
	}
	LedObject->sprite.setScale(Vector2f(0.8f, 0.6f));
	LedObject->next = objectsHead;
	objectsHead = LedObject;
}

void Simulator::createWire(Vertex line0, Pin* pin0, bool param_locked)
{
	Wire* WireObject = new Wire(line0, pin0, param_locked, *window);
	WireObject->next = objectsHead;
	objectsHead = WireObject;
}

void Simulator::deleteObject()
{
	Object* ptr = objectsHead;
	Object* temp = NULL;
	if (ptr->selected) //if head of list should be deleted
	{
		objectsHead = objectsHead->next;
		delete ptr;
		return;
	}
	else
	{
		while (ptr)
		{
			if (ptr->next->selected)
			{
				temp = ptr->next;
				ptr->next = ptr->next->next;
				delete temp;
				return;
			}
			ptr = ptr->next;
		}
	}
}

int Simulator::getNumberOfSelection()
{
	Object* ptr = objectsHead;
	int numOfSelection = 0;
	while (ptr)
	{
		if (ptr->selected)
		{
			numOfSelection++;
		}
		ptr = ptr->next;
	}
	return numOfSelection; //return the number of selected objects in the list
}

void Simulator::deleteAllSelectedObjects()
{
	int numOfSelection = getNumberOfSelection();
	for (int i = 0; i < numOfSelection; i++)
	{
		deleteObject(); // deleteObject() method deletes the first found, selected object in the list 
						// hence should be called for the number of selected object times
	}
}

void Simulator::drawAllObjects()
{
	Object* ptr = objectsHead;
	LogicElement* logicElement;
	Wire* wireObject;

	while (ptr) //if ptr is not NULL
	{
		logicElement = dynamic_cast<LogicElement*>(ptr);
		wireObject = dynamic_cast<Wire*>(ptr);

		if (!ptr->locked) //if object can move
		{
			if (logicElement) //if it is a logic element
			{
				logicElement->sprite.setPosition(Vector2f(Mouse::getPosition(*ptr->window)));
				logicElement->setElementPinsPositions();
			}

			else
			{
				Vertex vertex_line;
				vertex_line.position = Vector2f(Mouse::getPosition(*ptr->window));
				wireObject->setVertex(vertex_line);
			}
		}

		if (logicElement)
			window->draw(ptr->sprite);
		else  //wire
			window->draw(wireObject->getVertex(), 2, Lines);

		ptr = ptr->next;
	}
}

void Simulator::realeseObject()
{
	Object* ptr = objectsHead;
	while (ptr) //if ptr is not NULL
	{
		LogicElement* logicElement = dynamic_cast<LogicElement*>(ptr);
		Wire* wireObject = dynamic_cast<Wire*>(ptr);

		if (!(ptr->locked))
		{
			if (logicElement)
			{
				//if logic element is released at toolbar
				if (((ptr->sprite.getPosition().x < 120.0) || (ptr->sprite.getPosition().y < 60.0)))
				{
					ptr->selected = true;
					deleteObject();
					break;
				}
				//if logic element is dragged
				else if ((ptr->sprite.getPosition() == Vector2f(Mouse::getPosition(*ptr->window))))
					ptr->locked = true;
			}

			if (wireObject)
			{
				//if wire is released at toolbar
				if (((wireObject->getVertex()[1].position.x < 120.0) || (wireObject->getVertex()[1].position.y < 60.0)))
				{
					ptr->selected = true;
					deleteObject();
					break;
				}
				Object* ptr2 = objectsHead;
				while (ptr2)
				{
					int pinIndex = -1;
					LogicElement* tempLogic = dynamic_cast<LogicElement*>(ptr2);
					if (tempLogic)
						pinIndex = tempLogic->isItPin(Mouse::getPosition(*ptr->window).x, Mouse::getPosition(*ptr->window).y);
					if (pinIndex != -1) //if wire is released around a pin
					{
						wireObject->setPin(1, &(tempLogic->pins[pinIndex]));

						if (wireObject->getPin(0) == wireObject->getPin(1))//if wire is connected to same pin
						{
							ptr->selected = true;
							deleteObject();
							return;
						}
						//set ending point of wire
						Vertex vertex_line;
						vertex_line.position = Vector2f(Mouse::getPosition(*ptr->window));
						wireObject->setVertex(vertex_line);
						ptr->locked = true;
						return;
					}

					ptr2 = ptr2->next;
				}
				ptr->selected = true;
				deleteObject();
				break;
			}
		}
		ptr = ptr->next;
	}
}

void Simulator::clickExistingObject(LogicElement* logicElement)
{
	int pinIndex = logicElement->isItPin(Mouse::getPosition(*window).x, Mouse::getPosition(*window).y); //check if there is a pin around given position
	if ((logicElement->locked) && (pinIndex != -1)) //if there is a pin, create wire which starts from this pin
	{
		cout << "Pin is clicked" << endl;
		Vertex vertex_line;
		vertex_line.position = Vector2f(Mouse::getPosition(*window));
		createWire(vertex_line, &(logicElement->pins[pinIndex]), false);
	}
	else //draw a red rectangle around logic element and select this logic element for deletion
	{
		showRectangleAroundLogicElement(logicElement);
		logicElement->selected = true;
	}
}

void Simulator::startSimulation()
{
	//simulation should start from either gnd, vdd or clk

	Object* ptr = objectsHead;

	while (ptr)
	{
		Vdd* vddObject = dynamic_cast<Vdd*>(ptr);
		Gnd* gndObject = dynamic_cast<Gnd*>(ptr);
		Clk* clkObject = dynamic_cast<Clk*>(ptr);
		//simulation should be restart for all vdd, gnd, clk
		if (vddObject)
		{
			simulateFromLogicElement(vddObject);
		}

		else if (gndObject)
		{
			simulateFromLogicElement(gndObject);
		}
		else if (clkObject)
			simulateFromLogicElement(clkObject);
		ptr = ptr->next;
	}
}

void Simulator::simulateFromLogicElement(LogicElement* logicEl)
{
	//first, simulate all wires connected to given element's output pins

	for (int i = 0; i < logicEl->numPins; i++)
	{
		if (!(logicEl->pins[i].isItInput()))
		{
			for (int j = 0; j < logicEl->pins[i].getConnectionNum(); j++)
			{
				logicEl->pins[i].getWire(j)->Simulate();
			}
		}
	}

	//then, simulate logic elements that are connected to given logic element and so on

	for (int i = 0; i < logicEl->numPins; i++)
	{
		if (!(logicEl->pins[i].isItInput()))
		{
			for (int j = 0; j < logicEl->pins[i].getConnectionNum(); j++)
			{
				Wire* w = dynamic_cast<Wire*>(logicEl->pins[i].getWire(j));
				Pin* pin = w->getPin(0)->isItInput() ? w->getPin(0) : w->getPin(1);

				Object* ptr = objectsHead;
				LogicElement* logicElement;

				while (ptr)
				{
					logicElement = dynamic_cast<LogicElement*>(ptr);

					if (logicElement)
					{
						for (int k = 0; k < logicElement->numPins; k++)
						{
							if (pin == &logicElement->pins[k])
							{
								logicElement->Simulate(); //simulate connected logic element
								if (logicEl != logicElement)
									simulateFromLogicElement(logicElement);

							}
						}
					}
					ptr = ptr->next;
				}
			}
		}
	}
}

LogicElement* Simulator::isThereAnyGate(const Vector2f& pos)
{
	Object* ptr = objectsHead;

	float object_x;
	float object_y;
	LogicElement* logicElement;
	float mouse_x = pos.x;
	float mouse_y = pos.y;

	while (ptr)
	{
		object_x = ptr->sprite.getPosition().x;
		object_y = ptr->sprite.getPosition().y;
		logicElement = dynamic_cast<LogicElement*>(ptr);
		if (logicElement) //for each logic element in the list, compare the logic element's position with mouse position
		{
			if ((abs(object_x - mouse_x) <= 50) && (abs(object_y - mouse_y) <= 40))
				return logicElement;
		}
		ptr = ptr->next;
	}
	return NULL;
}

void Simulator::showRectangleAroundLogicElement(LogicElement* logicElement)
{
	if (logicElement->sprite.getPosition().x < 120 || logicElement->sprite.getPosition().y < 60) //if given element is a toolbar item
		return;
	hideRedRectangle();
	red_rectangle.setPosition(logicElement->sprite.getPosition()); //set positions as given logic element will be surrounded
	logicElement->selected = true; // select logic element
	selectConnectedWires(logicElement); //select all wires connected to this logic element
}

void Simulator::hideRedRectangle()
{
	red_rectangle.setPosition(2000.f, 2000.f); //set position outside of window
	Object* ptr = objectsHead;
	while (ptr) //clear all selections
	{
		if (ptr->selected)
			ptr->selected = false;
		ptr = ptr->next;
	}
}

void Simulator::toggleClk()
{
	Object* ptr = objectsHead;
	while (ptr)
	{
		Clk* clkObject = dynamic_cast<Clk*>(ptr);
		if (clkObject)
		{
			LogicElement* logicElement = dynamic_cast<LogicElement*>(clkObject);
			if (logicElement->pins[0].getPinState() == 0)
				logicElement->pins[0].setPinState(1);
			else
				logicElement->pins[0].setPinState(0);
		}
		ptr = ptr->next;
	}
}

void Simulator::clearDrawDisplay()
{
	//clear 
	window->clear(Color::Green);

	//draw everything
	window->draw(rectangle_horizontal);
	window->draw(rectangle_vertical);
	window->draw(spriteStart);
	window->draw(spriteStop);
	window->draw(red_rectangle);
	drawAllObjects();

	// end the current frame
	window->display();
}

void Simulator::selectConnectedWires(LogicElement* logicElement)
{
	for (int i = 0; i < logicElement->numPins; i++) // find pins connected to given logic element
	{
		for (int j = 0; j < logicElement->pins[i].getConnectionNum(); j++) // find wires connected to these pins
		{
			if (logicElement->pins[i].getWire(j))
			{
				logicElement->pins[i].getWire(j)->selected = true; //select all wires connected to given element
			}
		}
	}
}