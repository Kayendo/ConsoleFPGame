#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <chrono>

using std::cout, std::endl;

int main() {
	int screen_height = 40;
	int screen_width = 120;


	float player_x = 8.0f;
	float player_y = 8.0f;
	float player_a = 0.0f;

	int map_width = 16;
	int map_height = 16;

	float fov = 3.14159 / 3; // ~60 deg
	float depths = 16.0f;

	wchar_t* screen = new wchar_t[screen_height * screen_width];
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD Bytes = 0;


	std::wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#........####..#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (1) {
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
		float float_elapsed_time = elapsed_time.count();

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			player_a -= 5.0f * float_elapsed_time;
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			player_a += 5.0f * float_elapsed_time;
		}
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			player_x += sinf(player_a) * 5.0f * float_elapsed_time;
			player_y += cosf(player_a) * 5.0f * float_elapsed_time;

			if (map[static_cast<int>(player_y) * map_width + static_cast<int>(player_x)] == '#') {
				player_x -= sinf(player_a) * 5.0f * float_elapsed_time;
				player_y -= cosf(player_a) * 5.0f * float_elapsed_time;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			player_x -= sinf(player_a) * 5.0f * float_elapsed_time;
			player_y -= cosf(player_a) * 5.0f * float_elapsed_time;

			if (map[static_cast<int>(player_y) * map_width + static_cast<int>(player_x)] == '#') {
				player_x += sinf(player_a) * 5.0f * float_elapsed_time;
				player_y += cosf(player_a) * 5.0f * float_elapsed_time;
			}
		}

		if ((GetAsyncKeyState(VK_ESCAPE) & 0x01)) {
			return 0;
		}

		for (int x = 0; x < screen_width; ++x) {
			float ray_a = (player_a - fov / 2) + (float)(x) / (float)(screen_width)*fov;

			float distance_to_wall = 0.0f;
			bool hit_wall = false;

			float eye_x = sinf(ray_a);
			float eye_y = cosf(ray_a);

			while (!hit_wall && distance_to_wall < depths) {
				distance_to_wall += 0.1f;

				int test_x = static_cast<int>(player_x + eye_x * distance_to_wall);
				int test_y = static_cast<int>(player_y + eye_y * distance_to_wall);

				if (test_x < 0 || test_y < 0 || test_x >= map_width || test_y >= map_height) {
					hit_wall = true;
					distance_to_wall = depths;
				}
				else if (map[test_y * map_width + test_x] == '#') {
					hit_wall = true;
				}

			}

			int ceiling = (float)(screen_height / 2.0) - screen_height / (float)distance_to_wall;
			int floor = screen_height - ceiling;

			short shade = ' ';
			if (distance_to_wall <= depths / 3.0f) shade = 0x2588;  
			else if (distance_to_wall < depths / 2.0f) shade = 0x2593;
			else if (distance_to_wall < depths / 1.5f) shade = 0x2592;
			else if (distance_to_wall < depths) shade = 0x2591; 
			else shade = ' ';

			for (int y = 0; y < screen_height; ++y) {
				if (y <= ceiling) {
					screen[y * screen_width + x] = ' ';
				}
				else if (y > ceiling && y <= floor) {
					screen[y * screen_width + x] = shade;
				}
				else {
					float b = 1.0f - (static_cast<float>(y) - screen_height / 2.0) / (static_cast<float>(screen_height) / 2.0);
					if (b < 0.25) shade = 0x2593;
					else if (b < 0.5) shade = 0x2592;
					else if (b < 0.75) shade = 0x2591;
					else if (b < 0.9) shade = ' ';
					else shade = ' ';
					screen[y * screen_width + x] = shade;
				}
			}

		}
		WriteConsoleOutputCharacter(Console, screen, screen_width * screen_height, { 0,0 }, &Bytes);
	}
	
}
