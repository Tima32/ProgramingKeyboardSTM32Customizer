#pragma once
#include <SFML/Graphics.hpp>
#include <SFMLSTGUI/Button/Button.hpp>
#include <SFMLSTGUI/CheckBox/CheckBox.hpp>
#include <SFMLSTGUI/LineEdit/LineEdit.hpp>

#include "..\..\ProgrammingKeyboardAPI\ProgrammingKeyboardAPI.hpp"
#include "..\CommandWindow\CommandWindow.h"

class MainWindow
{
public:
	MainWindow();
	~MainWindow();

	void autoStep();
	
	void openWindow();
	void closeWindow();

private:
	void step();
	void event();
	void draw();

	sf::RenderWindow window;
	sf::Event event_e;
	bool focus;
	sf::Font font;

	st::Button key[256];
	void keyInit();
	void keyEvent();
	void keyDraw();

	st::CheckBox key_mode_standart_cb;
	st::CheckBox key_mode_command_cb;
	st::CheckBox key_mode_script_cb;
	st::CheckBox key_mode_switch_cb;

	st::Button key_mode_command_b;
	CommandWindow command_window;

	st::LineEdit connect_keyboard_le;
	st::Button connect_keyboard_b;
	void connectKeyboard();
	size_t select_keyboard;
	uint8_t select_key;

	void loadKeySettings();
	void applyKeySettings();

	st::Button settings_apply;
	st::Button settings_reload;

	void setCommandData();
};