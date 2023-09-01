#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <fstream>
#include <array>
#include <chrono>
#include <thread>
/* RPG Clube da Luta do H8
Integrantes:
João Vítor Trogildo Foresti;
Carlos Alberto Monteiro da Cunha;
Gustavo de Oliveira Portas
*/
enum class Attack {
    Punch,
    Kick,
    FlyingKick,
    BellyBump
};

enum class Weight {
    Null = 0,
    Light = 1,
    Heavy = 2
};

struct Character {
    std::string name;
    int hp;
    Weight weight;
    int damageAdder = 0;
};

bool getRandom(double a) {
    std::random_device dev;
    int seed = dev();
    auto gen = std::mt19937(seed);
    std::discrete_distribution<int> dist({100-a,a});
    return dist(gen);
}

int getRandomTriplet() {
    std::random_device dev;
    int seed = dev();
    auto gen = std::mt19937(seed);
    std::uniform_int_distribution<int> dist(0, 2);
    return dist(gen);
}
int getRandomDuo() {
    std::random_device dev;
    int seed = dev();
    auto gen = std::mt19937(seed);
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(gen);
}
std::string getAttackName(Attack attack) {
    switch (attack) {
        case Attack::Punch:
            return "Soco";
        case Attack::Kick:
            return "Chute";
        case Attack::FlyingKick:
            return "Voadora";
        case Attack::BellyBump:
            return "Barrigada";
    }
    return " ";
}
bool getAttackEffectiveness(Attack attack) {
    switch (attack) {
        case Attack::Punch:
            return getRandom(90);
        case Attack::Kick:
            return getRandom(70);
        case Attack::FlyingKick:
            return getRandom(20);
        case Attack::BellyBump:
            return getRandom(5);
    }
    return 0;
}

Attack getRandomAttack(Weight weight) {
    switch (getRandomTriplet()) {
        case 0:
            return Attack::Punch;
        case 1:
            return Attack::Kick;
        case 2:
            if (weight == Weight::Light) {
                return Attack::FlyingKick;
            } else {
                return Attack::BellyBump;
            }
    }
    return Attack::Punch;
}

int getAttackDamage(Attack attack) {
    switch (attack) {
        case Attack::Punch:
            return 1;
        case Attack::Kick:
            return 2;
        case Attack::FlyingKick:
            return 5;
        case Attack::BellyBump:
            return 25;
    }
    return 1;
}

void performPlayerTurn(Character& player, Character& enemy) {
    Attack attack;
    std::string str;
    while (true) {
        std::cin >> str;
        bool invalid = 0;
        switch (str[0]) {
            case 's':
            case 'S':
                attack = Attack::Punch;
                break;
            case 'c':
            case 'C':
                attack = Attack::Kick;
                break;
            case 'v':
            case 'V':
                if (player.weight == Weight::Heavy) {
                    std::cout << "Ataque invalido para um personagem pesado." << '\n';
                    invalid = 1;
                }
                attack = Attack::FlyingKick;
                break;
            case 'b':
            case 'B':
                if (player.weight == Weight::Light) {
                    std::cout << "Ataque invalido para um personagem leve." << '\n';
                    invalid = 1;
                }
                attack = Attack::BellyBump;
                break;
            default:
                std::cout << "Ataque invalido." << '\n';
                invalid = 1;
        }
        if (invalid) {
            continue;
        }
        break;
    }
    if (getAttackEffectiveness(attack)) {
        int damage = getAttackDamage(attack) + player.damageAdder;
        enemy.hp -= damage;
        std::cout << "Voce usou " << getAttackName(attack) << " e deu " << damage << " de dano em " << enemy.name << ',' << '\n';
        std::cout << "A vida restante de " << enemy.name << " eh " << enemy.hp << '\n' << '\n';
        return;
    } else {
        std::cout << "Voce errou o ataque em " << enemy.name << '.' << '\n' << '\n';
        return;
    }
}
int performEnemyTurn(Character& enemy, std::array<Character, 3>& playerParty) {
    Attack attack;
    int i;
    bool getLowLifeKick = 0;
    bool getLowLifePunch = 0;
    while(true) {
        for(i = 0; i < 3; ++i) {
            if(playerParty[i].hp < 2 + enemy.damageAdder && playerParty[i].hp > 0) {
                getLowLifeKick = 1;
                break;
            }
            if(playerParty[i].hp < 1 + enemy.damageAdder && playerParty[i].hp > 0) {
                getLowLifePunch = 1;
                break;
            }
        }
        if (getLowLifePunch or getLowLifeKick)
            break;
        i = getRandomTriplet();
        if (playerParty[i].hp > 0) {
            break;
        }
    }
    if (getLowLifePunch) {
        attack = Attack::Punch;
    } else if (getLowLifeKick) {
        attack = Attack::Kick;
    } else {
        attack = getRandomAttack(enemy.weight);
    }
    if (getAttackEffectiveness(attack)) {
        int damage = getAttackDamage(attack) + enemy.damageAdder;
        playerParty[i].hp -= damage;
        std::cout << "Inimigo " << enemy.name << " usou " << getAttackName(attack) << " e deu " << damage << " de dano em " << playerParty[i].name << ',' << '\n';
        if (playerParty[i].hp > 0) 
            std::cout << "A vida restante de " << playerParty[i].name << " eh " << playerParty[i].hp << '\n' << '\n';
        return i;
    } else {
        std::cout << "Inimigo errou o ataque em " << playerParty[i].name << '.' << '\n' << '\n';
        return -1;
    }
}
void saveGameState(const std::string& filename, const std::array<Character, 3>& playerParty, const std::array<Character, 3>& enemyParty, int roundNumber) {
    std::ofstream saveFile(filename);
    if (saveFile.is_open()) {
        for (int i = 0; i < 3; ++i) {
            const Character& player = playerParty[i];
            const Character& enemy = enemyParty[i];
            
            saveFile << player.hp << ' ' << static_cast<int>(player.weight) << ' ' << player.damageAdder <<'\n' << player.name << '\n';
            saveFile << enemy.hp << ' ' << static_cast<int>(enemy.weight) << ' ' << enemy.damageAdder << '\n' << enemy.name << '\n';
        }
        saveFile << roundNumber;
        saveFile.close();
        std::cout << "Jogo salvo com sucesso." << '\n';
        std::cout << "Saindo do jogo..." << '\n';
    } else {
        std::cout << "Não foi possível salvar o arquivo." << '\n';
    }
}

int loadGameState(const std::string& filename, std::array<Character, 3>& playerParty, std::array<Character, 3>& enemyParty) {
    std::ifstream loadFile(filename);
    int roundNumber;
    if (loadFile.is_open()) {
        for (int i=0; i<3; ++i) {
            int a, b;
            loadFile >> playerParty[i].hp;
            loadFile >> a;
            playerParty[i].weight = static_cast<Weight>(a);
            loadFile >> playerParty[i].damageAdder;
            std::getline(loadFile >> std::ws, playerParty[i].name);
            loadFile >> enemyParty[i].hp;
            loadFile >> b;
            enemyParty[i].weight = static_cast<Weight>(b);
            loadFile >> enemyParty[i].damageAdder;
            std::getline(loadFile >> std::ws, enemyParty[i].name);
        }
        loadFile >> roundNumber;
        loadFile.close();
        std::cout << "Jogo carregado com sucesso." << '\n';
        return roundNumber;
    } else {
        std::cout << "Nao foi possivel carregar o arquivo. Tente novamente." << '\n';
    }
    return -1;
}
bool checkEndBattle(const std::array<Character, 3>& fighterArray) {
    for (int j = 0, counter = 0; j < 3; ++j) {
        if (fighterArray[j].hp > 0) {
            return 0;
        }
        ++counter;
        if (counter == 3)
            return 1;
    }
    return 0;
}

void getRandomEvent(std::array<Character, 3>& enemyParty, std::array<Character, 3>& playerParty) {
    std::random_device dev;
    int seed = dev();
    auto gen = std::mt19937(seed);
    std::discrete_distribution<int> dist({50,25,12.5,7.5,5});
    int indexDuo, indexTriplet;
    while (true) {
        indexDuo = getRandomDuo();
        indexTriplet = getRandomTriplet();
        if(indexDuo == 0) {
            if(playerParty[indexTriplet].hp > 0)
                break;
        }
        if(indexDuo == 1) {
            if(enemyParty[indexTriplet].hp > 0)
                break;
        }
    }
    switch(dist(gen)) {
        case 0:
            break;
        case 1:
            std::cout << "Faltou luz no H8!" << '\n';
            std::cout << "Aleatoriamente, um dos integrantes levou velva (liquido refrescante.)" << '\n';
            if (indexDuo == 1) {
                std::cout << "\033[38;2;200;0;0m" << enemyParty[indexTriplet].name << "\033[0m" << " levou uma velvada com 27 espirradas e perdeu 3 de vida." << '\n' << '\n';
                enemyParty[indexTriplet].hp -= 3;
            } else {
                std::cout << "\033[38;2;0;0;200m" << playerParty[indexTriplet].name << "\033[0m" << " levou uma velvada com 27 espirradas e perdeu 3 de vida." << '\n' << '\n';
                playerParty[indexTriplet].hp -= 3;   
            }
            break;
        case 2:
            std::cout << "O Tinky Winky esta doando uma bike de bizuleu!" << '\n';
            std::cout << "Aleatoriamente, um dos integrantes recebe a bike e tem sua moral destruída." << '\n';
            if (indexDuo == 1) {
                std::cout << "\033[38;2;200;0;0m" << enemyParty[indexTriplet].name << "\033[0m" << " perdeu 6 de hp da bike do Tinky." << '\n' << '\n';
                enemyParty[indexTriplet].hp -= 6;
            } else {
                std::cout << "\033[38;2;0;0;200m" << playerParty[indexTriplet].name << "\033[0m" << " perdeu 6 de hp da bike do Tinky." << '\n' << '\n';
                playerParty[indexTriplet].hp -= 6;
            }
            break;
        case 3:
            std::cout << "A ceia melou!" << '\n';
            std::cout << "Maome pro bixo Chuva! Todos os ganham +2 de dano de ataque em todos os ataques." << '\n' << '\n';
            for (int i = 0; i < 3; ++i) {
                enemyParty[i].damageAdder += 2;
                playerParty[i].damageAdder += 2;
            }
            break;
        case 4:
            if (indexDuo == 1) {
                std::cout << "\033[38;2;200;0;0m" << enemyParty[indexTriplet].name << "\033[0m" << " meteu o viradao na noite anterior, perdendo 1 de dano em todos os ataques." << '\n' << '\n';
                enemyParty[indexTriplet].damageAdder -= 1;
            } else {
                std::cout << "\033[38;2;0;0;200m" << playerParty[indexTriplet].name << "\033[0m" << " meteu o viradao na noite anterior, perdendo 1 de dano em todos os ataques." << '\n' << '\n';
                playerParty[indexTriplet].damageAdder -= 1;
            }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return;
}
int main() {
    std::array<Character, 3> playerParty;
    std::array<Character, 3> enemyParty;
    std::cout << "\033[38;2;200;0;0m";
    std::cout << "****************************************" << '\n';
    std::cout << "    Bem vindo ao Clube da Luta do H8!   " << '\n';
    std::cout << "Se prepare para uma batalha emocionante!" << '\n';
    std::cout << "****************************************" << '\n' << '\n';
    std::cout << "\033[0m";
    
    std::string saveFilename;
    std::cout << "Coloque o nome do arquivo savestate (sem .txt): ";
    std::cin >> saveFilename;
    saveFilename += ".txt";
    int roundNumber = loadGameState(saveFilename, playerParty, enemyParty);
    int saveroundNumber = roundNumber;
    bool winBool, loseBool = 0;
    while (winBool == 0 && loseBool == 0) {
        int optionIndex = 1;
        if (roundNumber == -1)
            break;
        for (int i = roundNumber; i < 3; ++i) {
            Character& player = playerParty[i];
            if (player.hp <= 0)
                continue;
            std::cout << "Turno: " << "\033[38;2;0;0;200m" << player.name << "\033[0m" << " com " << player.hp << " de vida." << '\n';
            std::cout << "Escolha um lutador para atacar, ou digite 0 para salvar e sair" << '\n';
            for (int j = 0; j < 3; ++j) {
                if (enemyParty[j].hp > 0)
                    std::cout << std::setw(10) << enemyParty[j].name << ' ' << '(' << j+1 << ')' << " com " << enemyParty[j].hp << " de vida restante." << '\n';
            }
            std::cin >> optionIndex ;
            if (optionIndex == 0) {
                saveroundNumber = i;
                break;
            }
            Character& enemyTarget = enemyParty[optionIndex - 1];
            std::cout << "Lutador " << player.name << " vs. Inimigo " << enemyTarget.name << '\n';
            std::cout << "Escolha um ataque: [S]oco , [C]hute, [V]oadora (leve), [B]arrigada (pesado)" << '\n';
            performPlayerTurn(player, enemyTarget);
            if (enemyTarget.hp <= 0) {
                std::cout << "Inimigo " << enemyTarget.name << " abatido!" << '\n' << '\n';
            }
            saveroundNumber = i + 1;
            winBool = checkEndBattle(enemyParty);
            if(winBool) 
                break;
        }
        if (optionIndex  == 0)
            break;
        getRandomEvent(enemyParty, playerParty);
        for (int i = 0; i < 3; ++i) {
            Character& enemy = enemyParty[i];
            if (enemy.hp > 0) {
                std::cout << "Turno : " << "\033[38;2;200;0;0m" << enemy.name << "\033[0m" << " com " << enemy.hp << " de vida." <<'\n';
                int roundState = performEnemyTurn(enemy, playerParty);
                if (playerParty[roundState].hp <= 0 && roundState != -1) {
                    std::cout << "Lutador " << playerParty[roundState].name << " abatido!" << '\n';
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            loseBool = checkEndBattle(playerParty);
            if(loseBool)
                break;
        }
        if (optionIndex  != 0) {
            roundNumber = 0;
        } else {
            break;
        }
        getRandomEvent(enemyParty, playerParty);
    }
    if (winBool) {
        std::cout << "Parabens, Bixaral venceu!!" << '\n';
    } else if (loseBool) {
        std::cout << "X*** Bixaral!!" << '\n';
    }
    if (roundNumber != -1)
        saveGameState(saveFilename, playerParty, enemyParty, saveroundNumber);
    return 0;
}