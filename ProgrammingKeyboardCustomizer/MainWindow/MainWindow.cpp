#include "MainWindow.hpp"
#include "..\Keyboards\Keyboards.hpp"
#include <iostream>

using namespace std;
using namespace sf;
using namespace st;

MainWindow::MainWindow() :
	event_e(), focus(false), select_keyboard(-1), select_key(0)
{
	string s("Fonts\\verdana.ttf");
	if (!font.loadFromFile(s))
		cout << "Error load font" << endl;

	keyInit();

	key_mode_standart_cb.setPosition(20, 280);
	key_mode_standart_cb.setFont(font);
	key_mode_standart_cb.setString(L"Стандарт");
	key_mode_standart_cb.setState(st::CheckBox::State::Mixed);

	key_mode_command_cb.setPosition(20, 300);
	key_mode_command_cb.setFont(font);
	key_mode_command_cb.setString(L"Команда");
	key_mode_command_cb.setState(st::CheckBox::State::Mixed);

	key_mode_script_cb.setPosition(20, 320);
	key_mode_script_cb.setFont(font);
	key_mode_script_cb.setString(L"Скрипт");
	key_mode_script_cb.setState(st::CheckBox::State::Mixed);

	key_mode_switch_cb.setPosition(20, 340);
	key_mode_switch_cb.setFont(font);
	key_mode_switch_cb.setString(L"Переключение");
	key_mode_switch_cb.setState(st::CheckBox::State::Mixed);

	key_mode_command_b.setFont(font);
	key_mode_command_b.setText(L"Настроить...");
	key_mode_command_b.setPosition(key_mode_command_cb.getPosition().x + key_mode_command_cb.getSize().x + 50, key_mode_command_cb.getPosition().y);
	key_mode_command_b.setSize(key_mode_command_b.getSize().x, key_mode_command_cb.getSize().y);
	command_window.setFont(font);


	connect_keyboard_le.setFont(font);
	connect_keyboard_le.setPosition(400, 20);
	connect_keyboard_le.setString(L"COM20");
	connect_keyboard_b.setFont(font);
	connect_keyboard_b.setText(L"Подключить");
	connect_keyboard_b.setPosition(connect_keyboard_le.getPosition().x + connect_keyboard_le.getSize().x + 2, connect_keyboard_le.getPosition().y);
	connect_keyboard_b.setSize(connect_keyboard_b.getSize().x, connect_keyboard_le.getSize().y);


	//Кнопки применения и перезагрузки
	settings_apply.setFont(font);
	settings_reload.setFont(font);
	settings_apply.setText(L"Применить");
	settings_reload.setText(L"Перезагрузить");
	settings_apply.setPosition(604, 340);
	settings_reload.setPosition(settings_apply.getPosition().x + settings_apply.getSize().x + 2, settings_apply.getPosition().y);

	openWindow();
}
MainWindow::~MainWindow()
{
}

void MainWindow::autoStep()
{
	while (window.isOpen())
		step();
}

void MainWindow::openWindow()
{
	window.create(VideoMode(810, 370), L"KeyboardCcustomizer   (づ｡◕‿‿◕｡)づ");
	window.setFramerateLimit(60);
}
void MainWindow::closeWindow()
{
	window.close();
}

//private
void MainWindow::step()
{
	while (window.pollEvent(event_e))
		event();
	draw();

	command_window.step();
}
void MainWindow::event()
{
	if (event_e.type == Event::Resized)
	{
		auto view = window.getView();
		view.reset(FloatRect(0, 0, float(event_e.size.width), float(event_e.size.height)));
		window.setView(view);
	}
	if (event_e.type == Event::Closed)
	{
		window.close();
	}

	focus = false;

	if (settings_reload.event(event_e, focus))
	{
		loadKeySettings();
	}
	if (settings_apply.event(event_e, focus))
	{
		applyKeySettings();
	}

	//подключить клавиатуру
	if (connect_keyboard_b.event(event_e, focus))
	{
		connectKeyboard();
		loadKeySettings();
	}

	connect_keyboard_le.event(event_e, focus);

	if (key_mode_command_b.event(event_e, focus))
	{
		if (select_keyboard == -1)
			MessageBoxW(nullptr, L"Клавиатура не выбрана", L"Ошибка", MB_OK);
		else
			command_window.open(select_keyboard, select_key);
	}

	//изменение режима
	bool key_mode_event_b = false;
	key_mode_event_b |= key_mode_switch_cb.event(event_e, focus);
	key_mode_event_b |= key_mode_script_cb.event(event_e, focus);
	key_mode_event_b |= key_mode_command_cb.event(event_e, focus);
	key_mode_event_b |= key_mode_standart_cb.event(event_e, focus);
	if (key_mode_event_b && select_keyboard != -1)
	{
		uint8_t m = 0;
		if (key_mode_standart_cb.getState() == CheckBox::State::Checked)
			m |= TPKAPI::Data::Mode::Standart;
		if (key_mode_command_cb.getState() == CheckBox::State::Checked)
			m |= TPKAPI::Data::Mode::Command;
		if (key_mode_script_cb.getState() == CheckBox::State::Checked)
			m |= TPKAPI::Data::Mode::Send;
		if (key_mode_switch_cb.getState() == CheckBox::State::Checked)
			m |= TPKAPI::Data::Mode::SwitchMode;

		Keyboards::key_mode[select_key] = m;
	}

	keyEvent();
}
void MainWindow::draw()
{
	window.clear(Color(30, 30, 30));

	keyDraw();

	window.draw(key_mode_standart_cb);
	window.draw(key_mode_command_cb);
	window.draw(key_mode_script_cb);
	window.draw(key_mode_switch_cb);

	window.draw(key_mode_command_b);

	window.draw(connect_keyboard_le);
	window.draw(connect_keyboard_b);

	window.draw(settings_apply);
	window.draw(settings_reload);

	window.display();
}

//инициализация кнопок клавиатуры
void MainWindow::keyInit()
{
	static const Vector2f line0(20, 50);
	static const float b_size = 30;
	static const float b_indent = 5;
	static const float b_litter_shift = 18;
	static const float b_separator = 5;

	for (uint8_t i = 0; i < 0xE8; i++)
	{
		key[i].setPosition(0, -100);
		key[i].setAutoSize(false);
		key[i].setSize(b_size, b_size);
		key[i].setFont(font);
		key[i].setCharacterSize(11);
	}


	//нулевая линия
	{
		key[0x29].setPosition(line0);
		key[0x29].setText(L"Esc");

		key[0x3A].setPosition(line0.x + b_size * 2 + b_indent * 2, line0.y);
		key[0x3A].setText(L"F1");

		key[0x3B].setPosition(line0.x + b_size * 3 + b_indent * 3, line0.y);
		key[0x3B].setText(L"F2");

		key[0x3C].setPosition(line0.x + b_size * 4 + b_indent * 4, line0.y);
		key[0x3C].setText(L"F3");

		key[0x3D].setPosition(line0.x + b_size * 5 + b_indent * 5, line0.y);
		key[0x3D].setText(L"F4");

		key[0x3E].setPosition(line0.x + b_size * 6 + b_indent * 6 + (b_size + b_indent) / 2, line0.y);
		key[0x3E].setText(L"F5");

		key[0x3F].setPosition(line0.x + b_size * 7 + b_indent * 7 + (b_size + b_indent) / 2, line0.y);
		key[0x3F].setText(L"F6");

		key[0x40].setPosition(line0.x + b_size * 8 + b_indent * 8 + (b_size + b_indent) / 2, line0.y);
		key[0x40].setText(L"F7");

		key[0x41].setPosition(line0.x + b_size * 9 + b_indent * 9 + (b_size + b_indent) / 2, line0.y);
		key[0x41].setText(L"F8");

		key[0x42].setPosition(line0.x + b_size * 11 + b_indent * 11, line0.y);
		key[0x42].setText(L"F9");

		key[0x43].setPosition(line0.x + b_size * 12 + b_indent * 12, line0.y);
		key[0x43].setText(L"F10");

		key[0x44].setPosition(line0.x + b_size * 13 + b_indent * 13, line0.y);
		key[0x44].setText(L"F11");

		key[0x45].setPosition(line0.x + b_size * 14 + b_indent * 14, line0.y);
		key[0x45].setText(L"F12");
	}

	//первая линия
	static const Vector2f line1(line0.x, line0.y + b_size + b_indent + b_separator);
	{
		key[0x35].setPosition(line1);
		key[0x35].setText(L"~\n`");

		key[0x1E].setPosition(key[0x35].getPosition().x + b_size + b_indent, line1.y);
		key[0x1E].setText(L"1");

		key[0x1F].setPosition(key[0x1E].getPosition().x + b_size + b_indent, line1.y);
		key[0x1F].setText(L"2");

		key[0x20].setPosition(key[0x1F].getPosition().x + b_size + b_indent, line1.y);
		key[0x20].setText(L"3");

		key[0x21].setPosition(key[0x20].getPosition().x + b_size + b_indent, line1.y);
		key[0x21].setText(L"4");

		key[0x22].setPosition(key[0x21].getPosition().x + b_size + b_indent, line1.y);
		key[0x22].setText(L"5");

		key[0x23].setPosition(key[0x22].getPosition().x + b_size + b_indent, line1.y);
		key[0x23].setText(L"6");

		key[0x24].setPosition(key[0x23].getPosition().x + b_size + b_indent, line1.y);
		key[0x24].setText(L"7");

		key[0x25].setPosition(key[0x24].getPosition().x + b_size + b_indent, line1.y);
		key[0x25].setText(L"8");

		key[0x26].setPosition(key[0x25].getPosition().x + b_size + b_indent, line1.y);
		key[0x26].setText(L"9");

		key[0x27].setPosition(key[0x26].getPosition().x + b_size + b_indent, line1.y);
		key[0x27].setText(L"0");

		key[0x2D].setPosition(key[0x27].getPosition().x + b_size + b_indent, line1.y);
		key[0x2D].setText(L"-");

		key[0x2E].setPosition(key[0x2D].getPosition().x + b_size + b_indent, line1.y);
		key[0x2E].setText(L"+\n=");

		key[0x2A].setPosition(key[0x2E].getPosition().x + b_size + b_indent, line1.y);
		key[0x2A].setSize(b_size * 2 + b_indent, b_size); key[0x2A].setText(L"<—\nBackspace");
	}

	//вторая линия
	static const Vector2f line2(line1.x, line1.y + b_size + b_indent);
	{
		key[0x2B].setPosition(line2);
		key[0x2B].setSize(b_size + b_litter_shift, b_size); key[0x2B].setText(L"<— Tab\n—>");

		key[0x14].setPosition(key[0x2B].getPosition().x + key[0x2B].getSize().x + b_indent, line2.y);
		key[0x14].setText(L"Q");

		key[0x1A].setPosition(key[0x14].getPosition().x + b_size + b_indent, line2.y);
		key[0x1A].setText('W');

		key[0x08].setPosition(key[0x1A].getPosition().x + b_size + b_indent, line2.y);
		key[0x08].setText('E');

		key[0x15].setPosition(key[0x08].getPosition().x + b_size + b_indent, line2.y);
		key[0x15].setText('R');

		key[0x17].setPosition(key[0x15].getPosition().x + b_size + b_indent, line2.y);
		key[0x17].setText('T');

		key[0x1C].setPosition(key[0x17].getPosition().x + b_size + b_indent, line2.y);
		key[0x1C].setText('Y');

		key[0x18].setPosition(key[0x1C].getPosition().x + b_size + b_indent, line2.y);
		key[0x18].setText('U');

		key[0x0C].setPosition(key[0x18].getPosition().x + b_size + b_indent, line2.y);
		key[0x0C].setText('I');

		key[0x12].setPosition(key[0x0C].getPosition().x + b_size + b_indent, line2.y);
		key[0x12].setText('O');

		key[0x13].setPosition(key[0x12].getPosition().x + b_size + b_indent, line2.y);
		key[0x13].setText('P');

		key[0x2F].setPosition(key[0x13].getPosition().x + b_size + b_indent, line2.y);
		key[0x2F].setText("{\n[");

		key[0x30].setPosition(key[0x2F].getPosition().x + b_size + b_indent, line2.y);
		key[0x30].setText("}\n]");
	}

	//третья линия
	static const Vector2f line3(line2.x, line2.y + b_size + b_indent);{
		key[0x39].setPosition(line3);
		key[0x39].setSize(b_size + b_litter_shift * 2, b_size); key[0x39].setText(L"Caps\nLock");

		key[0x04].setPosition(key[0x39].getPosition().x + key[0x39].getSize().x + b_indent, line3.y);
		key[0x04].setText('A');

		key[0x16].setPosition(key[0x04].getPosition().x + b_size + b_indent, line3.y);
		key[0x16].setText('S');

		key[0x07].setPosition(key[0x16].getPosition().x + b_size + b_indent, line3.y);
		key[0x07].setText('D');

		key[0x09].setPosition(key[0x07].getPosition().x + b_size + b_indent, line3.y);
		key[0x09].setText('F');

		key[0x0A].setPosition(key[0x09].getPosition().x + b_size + b_indent, line3.y);
		key[0x0A].setText('G');

		key[0x0B].setPosition(key[0x0A].getPosition().x + b_size + b_indent, line3.y);
		key[0x0B].setText('H');

		key[0x0D].setPosition(key[0x0B].getPosition().x + b_size + b_indent, line3.y);
		key[0x0D].setText('J');

		key[0x0E].setPosition(key[0x0D].getPosition().x + b_size + b_indent, line3.y);
		key[0x0E].setText('K');

		key[0x0F].setPosition(key[0x0E].getPosition().x + b_size + b_indent, line3.y);
		key[0x0F].setText('L');

		key[0x33].setPosition(key[0x0F].getPosition().x + b_size + b_indent, line3.y);
		key[0x33].setText(":\n;");

		key[0x31].setPosition(key[0x33].getPosition().x + b_size + b_indent, line3.y);
		key[0x31].setText("\\\n|");

		key[0x28].setPosition(key[0x31].getPosition().x + b_size + b_indent, line3.y);
		key[0x28].setSize(key[0x2A].getPosition().x + key[0x2A].getSize().x - key[0x28].getPosition().x, b_size); key[0x28].setText("Enter");
	}

	//четвертая линия 
	static const Vector2f line4(line3.x, line3.y + b_size + b_indent);
	{
		key[0xE1].setPosition(line4);
		key[0xE1].setSize(b_size + b_litter_shift * 3 - (b_size + b_indent), b_size); key[0xE1].setText(L"Shift");

		key[0x32].setPosition(key[0xE1].getPosition().x + key[0xE1].getSize().x + b_indent, line4.y);
		key[0x32].setText("\\\n|");

		key[0x1D].setPosition(key[0x32].getPosition().x + b_size + b_indent, line4.y);
		key[0x1D].setText('Z');

		key[0x1B].setPosition(key[0x1D].getPosition().x + b_size + b_indent, line4.y);
		key[0x1B].setText('X');

		key[0x06].setPosition(key[0x1B].getPosition().x + b_size + b_indent, line4.y);
		key[0x06].setText('C');

		key[0x19].setPosition(key[0x06].getPosition().x + b_size + b_indent, line4.y);
		key[0x19].setText('V');

		key[0x05].setPosition(key[0x19].getPosition().x + b_size + b_indent, line4.y);
		key[0x05].setText('B');

		key[0x11].setPosition(key[0x05].getPosition().x + b_size + b_indent, line4.y);
		key[0x11].setText('N');

		key[0x10].setPosition(key[0x11].getPosition().x + b_size + b_indent, line4.y);
		key[0x10].setText('M');

		key[0x36].setPosition(key[0x10].getPosition().x + b_size + b_indent, line4.y);
		key[0x36].setText("<\n,");

		key[0x37].setPosition(key[0x36].getPosition().x + b_size + b_indent, line4.y);
		key[0x37].setText(">\n.");

		key[0x38].setPosition(key[0x37].getPosition().x + b_size + b_indent, line4.y);
		key[0x38].setText("?\n/");

		key[0xE5].setPosition(key[0x38].getPosition().x + b_size + b_indent, line4.y);
		key[0xE5].setSize(key[0x2A].getPosition().x + key[0x2A].getSize().x - key[0xE5].getPosition().x, b_size); key[0xE5].setText("Shift");
	}

	//пятая линия
	static const Vector2f line5(line4.x, line4.y + b_size + b_indent);
	static const Vector2f line4_b_size(key[0xE1].getSize().x, b_size);
	{
		key[0xE0].setPosition(line5);
		key[0xE0].setSize(line4_b_size); key[0xE0].setText(L"Ctrl");

		key[0xE3].setPosition(key[0xE0].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0xE3].setSize(line4_b_size); key[0xE3].setText(L"GUI");

		key[0xE2].setPosition(key[0xE3].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0xE2].setSize(line4_b_size); key[0xE2].setText(L"Alt");

		/*key[0x2C].setPosition(key[0xE2].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0x2C].setSize(line4_b_size); key[0x2C].setText(L"Space");*/

		/*key[0xE6].setPosition(key[0x2C].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0xE6].setSize(line4_b_size); key[0xE6].setText(L"Alt");

		key[0xE7].setPosition(key[0xE6].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0xE7].setSize(line4_b_size); key[0xE7].setText(L"GUI");

		key[0x65].setPosition(key[0xE7].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0x65].setSize(line4_b_size); key[0x65].setText(L"Menu");

		key[0xE4].setPosition(key[0x65].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0xE4].setSize(line4_b_size); key[0xE4].setText(L"Ctrl");*/
		//key[0x4C].getPosition().x + key[0x4C].getSize().x

		key[0xE4].setPosition(key[0x2A].getPosition().x + key[0x2A].getSize().x - line4_b_size.x, line5.y);
		key[0xE4].setSize(line4_b_size); key[0xE4].setText(L"Ctrl");

		key[0x65].setPosition(key[0xE4].getPosition().x - line4_b_size.x - b_indent, line5.y);
		key[0x65].setSize(line4_b_size); key[0x65].setText(L"Menu");

		key[0xE7].setPosition(key[0x65].getPosition().x - line4_b_size.x - b_indent, line5.y);
		key[0xE7].setSize(line4_b_size); key[0xE7].setText(L"GUI");

		key[0xE6].setPosition(key[0xE7].getPosition().x - line4_b_size.x - b_indent, line5.y);
		key[0xE6].setSize(line4_b_size); key[0xE6].setText(L"Alt");

		key[0x2C].setPosition(key[0xE2].getPosition().x + line4_b_size.x + b_indent, line5.y);
		key[0x2C].setSize(key[0xE6].getPosition().x - key[0x2C].getPosition().x - b_indent, line4_b_size.y); key[0x2C].setText(L"Space");
	}

	//Вспомогательные клавиши & Стрелки
	{
		key[0x46].setPosition(key[0x45].getPosition().x + key[0x45].getSize().x + b_indent + b_separator, line0.y);
		key[0x46].setText("Print\nScr");

		key[0x47].setPosition(key[0x46].getPosition().x + b_size + b_indent, line0.y);
		key[0x47].setText("Scrol\nLock");

		key[0x48].setPosition(key[0x47].getPosition().x + b_size + b_indent, line0.y);
		key[0x48].setText("Scrol\nLock");

		key[0x49].setPosition(key[0x46].getPosition().x, line1.y);
		key[0x49].setText("Ins");

		key[0x4A].setPosition(key[0x49].getPosition().x + b_size + b_indent, line1.y);
		key[0x4A].setText("Home");

		key[0x4B].setPosition(key[0x4A].getPosition().x + b_size + b_indent, line1.y);
		key[0x4B].setText("Page\nUp");

		key[0x4C].setPosition(key[0x46].getPosition().x, line2.y);
		key[0x4C].setText("Del");

		key[0x4D].setPosition(key[0x49].getPosition().x + b_size + b_indent, line2.y);
		key[0x4D].setText("End");

		key[0x4E].setPosition(key[0x4A].getPosition().x + b_size + b_indent, line2.y);
		key[0x4E].setText("Page\nDown");

		//стрелки
		key[0x52].setPosition(key[0x49].getPosition().x + b_size + b_indent, line4.y);
		key[0x52].setCharacterSize(33); key[0x52].setText(wchar_t(0xA71B));

		key[0x50].setPosition(key[0x46].getPosition().x, line5.y);
		key[0x50].setText("<-");

		key[0x51].setPosition(key[0x49].getPosition().x + b_size + b_indent, line5.y);
		key[0x51].setCharacterSize(33); key[0x51].setText(wchar_t(0xA71C));

		key[0x4F].setPosition(key[0x4A].getPosition().x + b_size + b_indent, line5.y);
		key[0x4F].setText("->");
	}

	//Num pad
	{
		key[0x53].setPosition(key[0x48].getPosition().x + key[0x48].getSize().x + b_indent + b_separator, line1.y);
		key[0x53].setText("Num\nLock");

		key[0x54].setPosition(key[0x53].getPosition().x + b_size + b_indent, line1.y);
		key[0x54].setText('/');

		key[0x55].setPosition(key[0x54].getPosition().x + b_size + b_indent, line1.y);
		key[0x55].setText('*');

		key[0x56].setPosition(key[0x55].getPosition().x + b_size + b_indent, line1.y);
		key[0x56].setText('-');

		key[0x5F].setPosition(key[0x53].getPosition().x, line2.y);
		key[0x5F].setText('7');

		key[0x60].setPosition(key[0x53].getPosition().x + b_size + b_indent, line2.y);
		key[0x60].setText('8');

		key[0x61].setPosition(key[0x54].getPosition().x + b_size + b_indent, line2.y);
		key[0x61].setText('9');

		key[0x57].setPosition(key[0x55].getPosition().x + b_size + b_indent, line2.y);
		key[0x57].setSize(b_size, b_size * 2 + b_indent); key[0x57].setText('+');

		key[0x5C].setPosition(key[0x53].getPosition().x, line3.y);
		key[0x5C].setText('4');

		key[0x5D].setPosition(key[0x53].getPosition().x + b_size + b_indent, line3.y);
		key[0x5D].setText('5');

		key[0x5E].setPosition(key[0x54].getPosition().x + b_size + b_indent, line3.y);
		key[0x5E].setText('6');

		///
		key[0x59].setPosition(key[0x53].getPosition().x, line4.y);
		key[0x59].setText('1');

		key[0x5A].setPosition(key[0x53].getPosition().x + b_size + b_indent, line4.y);
		key[0x5A].setText('2');

		key[0x5B].setPosition(key[0x54].getPosition().x + b_size + b_indent, line4.y);
		key[0x5B].setText('3');

		key[0x58].setPosition(key[0x55].getPosition().x + b_size + b_indent, line4.y);
		key[0x58].setSize(b_size, b_size * 2 + b_indent); key[0x58].setText("Enter");

		key[0x62].setPosition(key[0x53].getPosition().x, line5.y);
		key[0x62].setSize(b_size * 2 + b_indent, b_size); key[0x62].setText('0');

		key[0x63].setPosition(key[0x54].getPosition().x + b_size + b_indent, line5.y);
		key[0x63].setText('.');
	}

}
void MainWindow::keyEvent()
{
	for (uint8_t i = 0; i < 0xE8; i++)
	{
		if (key[i].event(event_e, focus))
		{

			cout << "<MainWindow:keyEvent>:Info: Press key 0x" << std::hex << std::uppercase << (int)i << std::dec << endl;
			select_key = i;
			if (select_keyboard != -1)
			{
				auto mode = Keyboards::key_mode[i];
				if (mode & TPKAPI::Data::Mode::Standart)
					key_mode_standart_cb.setState(CheckBox::State::Checked);
				else
					key_mode_standart_cb.setState(CheckBox::State::Unchecked);

				if (mode & TPKAPI::Data::Mode::Command)
					key_mode_command_cb.setState(CheckBox::State::Checked);
				else
					key_mode_command_cb.setState(CheckBox::State::Unchecked);

				if (mode & TPKAPI::Data::Mode::Send)
					key_mode_script_cb.setState(CheckBox::State::Checked);
				else
					key_mode_script_cb.setState(CheckBox::State::Unchecked);

				if (mode & TPKAPI::Data::Mode::SwitchMode)
					key_mode_switch_cb.setState(CheckBox::State::Checked);
				else
					key_mode_switch_cb.setState(CheckBox::State::Unchecked);
			}
		}
	}
}
void MainWindow::keyDraw()
{
	for (uint8_t i = 0; i < 0xE8; i++)
	{
		window.draw(key[i]);
	}

	//window.draw(key[0x29]);
}

void MainWindow::connectKeyboard()
{
	if (connect_keyboard_le.getString().find(L"COM") == -1)
		return;

	wstring p = L"\\\\.\\";
	p += connect_keyboard_le.getString();

	for (size_t i = 0; i < Keyboards::keyboards.size(); i++)
	{
		if (Keyboards::keyboards[i]->getName().find(p) != -1)
		{
			delete Keyboards::keyboards[i];
			delete Keyboards::keyboards_m[i];
			Keyboards::keyboards.erase(Keyboards::keyboards.begin() + i);
			Keyboards::keyboards_m.erase(Keyboards::keyboards_m.begin() + i);
			select_keyboard = -1;
			wcout << L"<MaainWindow:event>:Info: Keyboard " << p << L" disconnected" << endl;
			return;
		}

	}

	TPKAPI::ProgrammingKeyboard* k = new TPKAPI::ProgrammingKeyboard;
	k->open(p.c_str());
	if (!k->isOpen())
	{
		delete k;
		wcout << L"<MaainWindow:event>:Error: Error connect to " << p << endl;
		return;
	}
	Keyboards::keyboards.push_back(k);
	Keyboards::keyboards_m.push_back(new mutex);
	select_keyboard = Keyboards::keyboards.size() - 1;
	wcout << L"<MaainWindow:event>:Info: Keyboard " << p << L" connected" << endl;
}
void MainWindow::loadKeySettings()
{
	if (select_keyboard == -1)
		return;

	key_mode_standart_cb.setState(st::CheckBox::State::Mixed);
	key_mode_command_cb.setState(st::CheckBox::State::Mixed);
	key_mode_script_cb.setState(st::CheckBox::State::Mixed);
	key_mode_switch_cb.setState(st::CheckBox::State::Mixed);
	select_key = 0;

	for (uint8_t i = 0; i < 0xE8; i++)
	{
		Keyboards::key_mode[i] = Keyboards::keyboards[select_keyboard]->getKeyMode(i);
	}
}
void MainWindow::applyKeySettings()
{
	for (uint8_t i = 0; i < 0xE8; i++)
	{
		Keyboards::keyboards[select_keyboard]->setKeyMode(i, TPKAPI::Data::Mode(Keyboards::key_mode[i]));
	}
	setCommandData();
}

void MainWindow::setCommandData()
{
	TPKAPI::Data::CommandData cd[4096];
	uint16_t* cda = reinterpret_cast<uint16_t*>(cd);
	memset(cd, 0, 4096 * 2);
	uint16_t pos = 0;

	for (uint8_t key = 0; key < 0xE8; key++)
	{
		if (Keyboards::key_press_com[key].size())
		{
			Keyboards::keyboards[select_keyboard]->setPressMap(key, pos);
			for (size_t i = 0; i < Keyboards::key_press_com[key].size(); i++, pos++)
			{
				cd[pos] = Keyboards::key_press_com[key][i];
			}
		}
		if (Keyboards::key_release_com[key].size())
		{
			Keyboards::keyboards[select_keyboard]->setReleaseMap(key, pos);
			for (size_t i = 0; i < Keyboards::key_release_com[key].size(); i++, pos++)
			{
				cd[pos] = Keyboards::key_release_com[key][i];
			}
		}
	}

	Keyboards::keyboards[select_keyboard]->setCommands(cd, pos);
}