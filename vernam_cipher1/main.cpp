#include "color.hpp"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;


namespace output {
	void info(const char* msg, ...) {
		std::cout << "[ " << dye::aqua("nick") << " ] ";

		va_list args;
		va_start(args, msg);
		vprintf(msg, args);
		va_end(args);

		std::cout << std::endl;
	}
	void error(const char* msg, ...) {
		std::cout << "[ " << dye::red("nick ANGRY") << " ] ";

		va_list args;
		va_start(args, msg);
		vprintf(msg, args);
		va_end(args);

		std::cout << std::endl;
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
void generate_pad_menu(int selected = 1);

bool is_input(int vKey) { 
	return /*(GetConsoleWindow() == GetForegroundWindow()) &&*/ (GetAsyncKeyState(vKey) & 1);
}

bool is_number(const char* inp) {
	for (int i = 0; i < strlen(inp); i++)
		if (!isdigit(inp[i])) return false;
	return true;
}

#define VK_N 0x4E
#define VK_Y 0x59

static std::string alphabet = " !\"#$%'()*+,-.0123456789:;<=>?@[]^_abcdefghijklmnopqrstuvwxyz{}~"; // for some reason causes errors when add upper alphabet

int get_pos_in_alphabet(char c) {
	for (int i = 0; i < alphabet.length(); i++)
		if (alphabet[i] == c) return i;
	return -1;
}

std::string generate_pad(int length = 24) {
	std::string pad = "";

	for (int i = 0; i < length; i++)
		pad += alphabet[rand() % alphabet.length()];

	return pad;
}

#undef max
// scenes:
scene generate_pad_scenes[2] = {
	scene("generate pads", []() {
		while (is_input(VK_RETURN)) Sleep(10);

		//std::cin.clear();
		//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
		//std::cin.ignore(10000, ' ');
		// ask for num of pads to generate
		int numGeneratedPads = -1;
		while (numGeneratedPads == -1) {
			std::cin.clear();
			output::info("how many pads do you want to generate?");
			std::string s;
			std::getline(std::cin, s);
			if (is_number(s.c_str()) && s.length() != 0)
				numGeneratedPads = atoi(s.c_str());
			else output::error("invalid input. should be positive whole number");
		}
		
		// ask for length of pads?
		int lengthOfPads = 24;

		// now we have number of pads to generate, ask for filename to save to (will be saved in local 'generated-pads' directory)
		output::info("enter filename to save to generated pads directory");
		std::string s;
		std::getline(std::cin, s);

		std::ofstream file("generated-pads/" + s + ".txt");
		if (file.fail()) {
			printf("failed\n"); return;
		}

		// generate pads
		for (int i = 0; i < numGeneratedPads; i++) {
			file << generate_pad(lengthOfPads);
			file << std::endl;
		}

		file.close();

		// output status, return to scene
		output::info("%d pads generated in file /generated-pads/%s.txt\n", numGeneratedPads, s.c_str());
		std::cin.get();
	}),
	scene("back", []() {
		main_menu(1);
	}),
};


scene main_menu_scenes[3] = {
	scene("encrypt / decrypt", []() {
		printf("hi"); 
	}),
	scene("generate one-time pad", []() {
		generate_pad_menu();
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

void display_options(scene* scenes, void (*option_func)(int selected), int selected, int length) {
	if (scenes == nullptr) return;

	for (int i = 0; i < length; i++) {
		scene s = scenes[i];
		int option = i + 1;
		output::option(option, s.get_title(), option == selected);
	}
	while (true) {
		if (is_input(VK_DOWN)) {
			system("cls");
			if (selected < 3) selected++;
			return option_func(selected);
		}
		else if (is_input(VK_UP)) {
			system("cls");
			if (selected > 1) selected--;
			return option_func(selected);
		}
		else if (is_input(VK_RETURN)) {
			//system("cls");
			std::cin.clear();
			scenes[selected - 1].on_select();
			break;
		}
		else if (is_input(VK_BACK)) {
			std::cin.clear(); // i think this fixes my issue of cin being annoying
			// search for a back option and execute
			for (int i = 0; i < length; i++) {
				scene s = scenes[i];
				if (s.get_title() == "back") return s.on_select();
			}
		}
	}
}

void main_menu(int selected = 1) {
	system("cls");
	output::info("welcome to my encryption program");
	output::info("use key arrows to navigate\n");

	display_options(main_menu_scenes, main_menu, selected, 3);
}

void generate_pad_menu(int selected) {
	system("cls");
	output::info("generate pads");
	output::info("use key arrows to navigate\n");

	display_options(generate_pad_scenes, generate_pad_menu, selected, 2);
}

int main() {
	SetConsoleTitle("nick's vernam cipher");

	fs::create_directory("generated-pads");

	std::ofstream file("generated-pads/filename.txt");
	if (file.fail()) printf("failed\n");
	file << std::hex << "ayo\n";
	file.close();


	//system("pause");

	/*std::string pad = "abc";
	std::string plaintext = "aaa";
	std::string ciphertext = "";

	for (int i = 0; i < plaintext.length(); i++) {
		
		printf("%02X %02X %02X\n", pad[i], plaintext[i], pad[i] ^ plaintext[i]);
		//ciphertext += (plaintext[i] ^ pad[i]);
	}*/

	//std::cout << ciphertext << std::endl;

	std::string plaintext = "hello";
	std::string salt = "bbcdasdsetrthfthfghtrgfdgdgrgdrgdrhuk8765rfgffffffffssssssseeeeb";
	std::string ciphertext = ""; // https://www.calcresult.com/misc/cyphers/vernam.html

	// i think this is a good implementation of vernam cipher?
	for (int i = 0; i < plaintext.length(); i++) {
		char plain_c = plaintext[i];
		char salt_c = salt[i];

		int plain_off = get_pos_in_alphabet(plain_c);
		int salt_off = get_pos_in_alphabet(salt_c);

		if (!(plain_off != -1 && salt_off != -1)) continue;

		int new_off = (plain_off + salt_off) % alphabet.length(); // change this to minus to decrypt

		ciphertext += alphabet[new_off];
	}

	printf("%s\n", ciphertext.c_str());


	printf("\n\n");
	//system("pause");
	
	

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