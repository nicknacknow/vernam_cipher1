#include "color.hpp"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;


namespace output {
	void info(const char* msg) {
		std::cout << "[ " << dye::aqua("nick") << " ] " << msg << std::endl;
	}
	void error(const char* msg) {
		std::cout << "[ " << dye::red("nick ANGRY") << " ] " << msg << std::endl;
	}

	void option(int option, const char* msg, bool selected) {
		std::cout << (selected ? hue::on_light_purple : hue::bright_white_on_grey) << "(" << (option) << ")"  << " " << msg << hue::reset << std::endl;
	}
}

class scene {
public:
	scene(const char* name, void (*f)()) : title(name), func(f) {}

	void on_select() {
		if (func) func();
	}

	const char* get_title() {
		return title;
	}

	void set_title(const char* new_title) {
		title = new_title;
	}
private:
	const char* title;
	void (*func)();
};

void main_menu(int selected);

bool is_input(int vKey) {
	return (GetConsoleWindow() == GetForegroundWindow()) && (GetAsyncKeyState(vKey) & 1);
}

#define VK_N 0x4E
#define VK_Y 0x59

// scenes:
scene main_menu_scenes[3] = {
	scene("encrypt / decrypt", []() {
		printf("hi"); 
	}),
	scene("generate one-time pad", []() {
		printf("hiya"); 
	}),
	scene("exit", []() {
		printf("are you sure? [y/n]\n");
		while (!is_input(VK_N)) {
			if (is_input(VK_Y)) { exit(0); }
		}

		Sleep(10); // annoying issue where keys pressed whilst waiting for input all load when cin is free
		main_menu(1);
	}) 
};

void main_menu(int selected = 1) {
	system("cls");
	output::info("welcome to my encryption program");
	output::info("use key arrows to navigate\n");

	for (int i = 0; i < 3; i++) {
		scene s = main_menu_scenes[i];
		int option = i + 1;
		output::option(option, s.get_title(), option == selected);
	}

	while (true) {
		if (is_input(VK_DOWN)) {
			system("cls");
			if (selected < 3) selected++;
			return main_menu(selected);
		}
		else if (is_input(VK_UP)) {
			system("cls");
			if (selected > 1) selected--;
			return main_menu(selected);
		}
		else if (is_input(VK_RETURN)) {
			//system("cls");
			main_menu_scenes[selected - 1].on_select();
			break;
		}
	}
}

void generate_pad_menu() {

}

int main() {
	SetConsoleTitle("nick's vernam cipher");

	fs::create_directory("generated-pads");

	std::ofstream file("generated-pads/filename.a", std::ios::binary);
	file << std::hex << "a\0yo\n";
	file.close();
	

	// filesystem
	

	std::string pad = "abc";
	std::string plaintext = "aaa";
	std::string ciphertext = "";

	for (int i = 0; i < plaintext.length(); i++) {
		
		printf("%02X %02X %02X\n", pad[i], plaintext[i], pad[i] ^ plaintext[i]);
		//ciphertext += (plaintext[i] ^ pad[i]);
	}

	//std::cout << ciphertext << std::endl;

	system("pause");
	
	

	/*output::info("welcome to my encryption program");
	output::info("use key arrows to navigate\n");

	output::option(1, "hi", true);
	output::option(2, "lol", false);
	output::option(3, "lolaaaa", false);
	
	auto a = []() {

	};
	a;*/

	main_menu();

	return 1;
}