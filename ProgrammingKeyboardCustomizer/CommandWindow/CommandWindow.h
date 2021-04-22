#pragma once
#include <SFML/Graphics.hpp>
#include <SFMLSTGUI/Button/Button.hpp>
#include <SFMLSTGUI/LineEdit/LineEdit.hpp>
#include <SFMLSTGUI/Radio/Radio.h>

class CommandWindow
{
public:
	CommandWindow();
	~CommandWindow();

	void setFont(const sf::Font& font);

	void open(size_t select_keyboard, uint8_t select_key);
	void close();

	void step();
private:
	void event();
	void draw();

	sf::RenderWindow window;
	sf::Event event_e;
	bool focus;

	st::LineEdit command_line;
	st::Radio press_r;
	st::Radio release_r;
	st::RadioController radio_controller;
	sf::Text key_code_t;
	st::LineEdit key_code_le;
	st::Button keyboard_press_b;
	st::Button keyboard_release_b;
	st::Button consumear_press_b;
	st::Button consumear_release_b;
	st::Button mouse_press_b;
	st::Button mouse_release_b;
	st::Button command_ok;

	size_t select_keyboard;
	uint8_t select_key;

	void parseCodeLine();
};