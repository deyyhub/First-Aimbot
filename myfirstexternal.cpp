#include "headers.h"


using namespace std;

class vector3 {
public:
    float x;
    float y;
    float z;
};

class view {
public:
    float pitch; //x
    float yaw;  //y
};

class player {
public:
    vector3 position;
    view direction;

    int health;
    char name[20];

};


int main()

{

    Memory mem(L"ac_client.exe");


    uintptr_t game = mem.GetModuleBaseAddress(L"ac_client.exe");

    /*
    uintptr_t plr = mem.Read<uintptr_t>(game + offsets::localplayer);


    player localPlayer;

    localPlayer.position = mem.Read<vector3>(plr + offsets::posX);
    localPlayer.health = mem.Read<int>(plr + offsets::health);
    localPlayer.direction = mem.Read<view>(plr + offsets::camX);

    cout << localPlayer.position.x << endl;
    cout << localPlayer.position.y << endl;
    cout << localPlayer.position.z << endl;

    cout << localPlayer.health << endl;
    

    cout << localPlayer.direction.pitch << endl;
    cout << localPlayer.direction.yaw << endl;


    uintptr_t entities = mem.Read<uintptr_t>(game + offsets::entitylist);


    uintptr_t count = mem.Read<int>(game + offsets::playercount);

    */


    while(true){
        
        uintptr_t currentTarget = 0;

        while (true) {
            uintptr_t plr = mem.Read<uintptr_t>(game + offsets::localplayer);

            // 1. Reset target if Right Click is released
            if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
                currentTarget = 0;
            }

            if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {

                // 2. If we have a target, check if they are still valid/alive
                if (currentTarget != 0) {
                    int health = mem.Read<int>(currentTarget + offsets::health);
                    if (health <= 0 || health > 100) {
                        currentTarget = 0; // Target is dead, reset so we can find a new one
                    }
                }

                // 3. If we DON'T have a target, find the closest one
                if (currentTarget == 0) {
                    float closestDistance = 999999.0f;
                    int playerCount = mem.Read<int>(game + offsets::playercount);
                    uintptr_t entityList = mem.Read<uintptr_t>(game + offsets::entitylist);

                    for (int i = 0; i < playerCount; i++) {
                        uintptr_t ent = mem.Read<uintptr_t>(entityList + (i * 0x4));
                        if (!ent || ent == plr) continue;

                        int health = mem.Read<int>(ent + offsets::health);
                        if (health <= 0 || health > 100) continue;

                        vector3 entpos = mem.Read<vector3>(ent + offsets::posX);
                        vector3 localPos = mem.Read<vector3>(plr + offsets::posX); // Read fresh local pos

                        float distance = sqrt(pow(entpos.x - localPos.x, 2) +
                            pow(entpos.y - localPos.y, 2) +
                            pow(entpos.z - localPos.z, 2));

                        if (distance < closestDistance) {
                            closestDistance = distance;
                            currentTarget = ent; // Lock this player as the 
                        }
                    }
                }

                // 4. If we successfully have a target (old or new), aim at them
                if (currentTarget != 0) {
                    vector3 entpos = mem.Read<vector3>(currentTarget + offsets::posX);
                    vector3 localPos = mem.Read<vector3>(plr + offsets::posX);

                    vector3 delta = { entpos.x - localPos.x, entpos.y - localPos.y, entpos.z - localPos.z };
                    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

                    float yaw = (float)(atan2(delta.y, delta.x) * (180 / M_PI)) + 90.0f;
                    float pitch = (float)(atan2(delta.z, hyp) * (180 / M_PI));

                    mem.Write<float>(plr + offsets::camX, yaw);
                    mem.Write<float>(plr + offsets::camY, pitch);
                }
            }
            Sleep(1); // Small sleep to prevent CPU hogging
        }
        
        
    }
}