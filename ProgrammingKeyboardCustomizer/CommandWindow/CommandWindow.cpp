#include <iostream>
#include "CommandWindow.h"

#include "..\Keyboards\Keyboards.hpp"

using namespace std;
using namespace sf;
using namespace st;

CommandWindow::CommandWindow():
	event_e(), focus(), select_keyboard(-1), select_key(0)
{
	command_line.setPosition(20, 20);
	command_line.setSize(370 - 40);

	press_r.setString(L"Нажатие");
	release_r.setString(L"Отжатие");
	radio_controller.arr.push_back(&press_r);
	radio_controller.arr.push_back(&release_r);
	radio_controller.alignHorizontally(Vector2f(20, command_line.getPosition().y + command_line.getSize().y + 5));

	key_code_t.setString(L"Код клавиши 0x");
	key_code_t.setCharacterSize(11);
	key_code_le.setSize(command_line.getPosition().x + command_line.getSize().x - key_code_le.getPosition().x);

	keyboard_press_b.setText(L"Нажать клавиатура");
	keyboard_release_b.setText(L"Отжать клавиатура");
	consumear_press_b.setText(L"Нажать мультимедия");
	consumear_release_b.setText(L"Отжать мультимедия");
	mouse_press_b.setText(L"Нажать мышь");
	mouse_release_b.setText(L"Отжать мышь");
	command_ok.setText(L"ОК");
}
CommandWindow::~CommandWindow()
{}

void CommandWindow::setFont(const sf::Font& font)
{
	command_line.setFont(font);

	press_r.setFont(font);
	release_r.setFont(font);
	key_code_t.setFont(font);
	key_code_le.setFont(font);

	keyboard_press_b.setFont(font);
	keyboard_release_b.setFont(font);
	consumear_press_b.setFont(font);
	consumear_release_b.setFont(font);
	mouse_press_b.setFont(font);
	mouse_release_b.setFont(font);
	command_ok.setFont(font);
}

void CommandWindow::open(size_t select_keyboard, uint8_t select_key)
{
	this->select_keyboard = select_keyboard;
	this->select_key = select_key;

	radio_controller.alignHorizontally(Vector2f(20, command_line.getPosition().y + command_line.getSize().y + 5));
	key_code_t.setPosition(release_r.getPosition().x + release_r.getSize().x + 5, release_r.getPosition().y + 1);
	key_code_le.setPosition(key_code_t.getPosition().x + key_code_t.getLocalBounds().width + 5, release_r.getPosition().y);
	key_code_le.setSize(command_line.getPosition().x + command_line.getSize().x - key_code_le.getPosition().x);

	keyboard_press_b.setPosition(20, press_r.getPosition().y + press_r.getSize().y + 5);
	keyboard_press_b.setSize((370-44) /2, command_line.getSize().y);
	keyboard_release_b.setPosition(keyboard_press_b.getPosition().x + keyboard_press_b.getSize().x + 4, press_r.getPosition().y + press_r.getSize().y + 5);
	keyboard_release_b.setSize((370 - 44) / 2, command_line.getSize().y);

	consumear_press_b.setPosition(keyboard_press_b.getPosition().x, keyboard_press_b.getPosition().y + keyboard_press_b.getSize().y + 5);
	consumear_press_b.setSize(keyboard_press_b.getSize());
	consumear_release_b.setPosition(keyboard_release_b.getPosition().x, keyboard_press_b.getPosition().y + keyboard_press_b.getSize().y + 5);
	consumear_release_b.setSize(keyboard_press_b.getSize());

	mouse_press_b.setPosition(keyboard_press_b.getPosition().x, consumear_press_b.getPosition().y + consumear_press_b.getSize().y + 5);
	mouse_press_b.setSize(keyboard_press_b.getSize());
	mouse_release_b.setPosition(keyboard_release_b.getPosition().x, consumear_press_b.getPosition().y + consumear_press_b.getSize().y + 5);
	mouse_release_b.setSize(keyboard_press_b.getSize());

	command_ok.setPosition(keyboard_press_b.getPosition().x, mouse_release_b.getPosition().y + mouse_release_b.getSize().y + 5);
	command_ok.setSize(command_line.getSize());

	window.create(VideoMode(370, 170), L"Настройка команды");
	window.setFramerateLimit(60);

	draw();
}
void CommandWindow::close()
{
	window.close();
}

void CommandWindow::step()
{
	while (window.pollEvent(event_e))
		event();
	draw();
}

//private
void CommandWindow::event()
{
	if (event_e.type == Event::Resized)
	{
		auto view = window.getView();
		view.reset(FloatRect(0, 0, float(event_e.size.width), float(event_e.size.height)));
		window.setView(view);
		command_line.setSize(event_e.size.width - 40);
	}
	if (event_e.type == Event::Closed)
	{
		window.close();
	}

	focus = false;

	if (command_ok.event(event_e, focus))
	{
		parseCodeLine();
	}
	mouse_release_b.event(event_e, focus);
	mouse_press_b.event(event_e, focus);
	consumear_release_b.event(event_e, focus);
	consumear_press_b.event(event_e, focus);
	keyboard_release_b.event(event_e, focus);
	keyboard_press_b.event(event_e, focus);

	key_code_le.event(event_e, focus);
	radio_controller.event(event_e, focus);
	command_line.event(event_e, focus);
}
void CommandWindow::draw()
{
	window.clear(Color(30, 30, 30));

	window.draw(command_line);
	window.draw(radio_controller);
	window.draw(key_code_t);
	window.draw(key_code_le);

	window.draw(keyboard_press_b);
	window.draw(keyboard_release_b);
	window.draw(consumear_press_b);
	window.draw(consumear_release_b);
	window.draw(mouse_press_b);
	window.draw(mouse_release_b);
	window.draw(command_ok);

	window.display();
}

void CommandWindow::parseCodeLine()
{
	std::vector<TPKAPI::Data::CommandData>* key_com = nullptr;
	if (press_r.getCheck())
		key_com = &Keyboards::key_press_com[select_key];
	if (release_r.getCheck())
		key_com = &Keyboards::key_release_com[select_key];

	if (key_com == nullptr)
	{
		MessageBoxW(nullptr, L"Выберите на какое действаие назначить команду", L"Ошибка", MB_OK);
		return;
	}

	key_com->clear();
	const wstring s = command_line.getString();
	wstring command;
	for (size_t i = 0; s[i] != L'\0'; i++)
	{
		if (s[i] == L' ')
		{
			if (command.size() > 4)
			{
				MessageBoxW(nullptr, L"Слишком длинная команда", L"Ошибка", MB_OK);
				return;
			}
			uint16_t c_i = stoull(command, nullptr, 16);
			TPKAPI::Data::CommandData c;
			c.action = TPKAPI::Data::CommandData::Action(c_i >> 8);
			c.key_code = (uint8_t)c_i;
			key_com->push_back(c);
			command.clear();
		}
		else
			command += s[i];
	}
	if (command.size())
	{
		uint16_t c_i = stoull(command, nullptr, 16);
		TPKAPI::Data::CommandData c;
		c.action = TPKAPI::Data::CommandData::Action(c_i >> 8);
		c.key_code = (uint8_t)c_i;
		key_com->push_back(c);
		command.clear();
	}

	uint16_t c_i = 0;
	TPKAPI::Data::CommandData* c = reinterpret_cast<TPKAPI::Data::CommandData*>(&c_i);
	key_com->push_back(*c);
}