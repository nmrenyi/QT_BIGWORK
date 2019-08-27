#include "command.h"


Command::Command() {

}

std::vector<int> Command::getPara() {
    return parametre;
}
void Command::setName(std::string s) {
    commandName = s;
}

void Command::setTime(int t) {
    time = t;
}

int Command::getTime() const{
    return time;
}

std::string Command::getName() {
    return commandName;
}

bool Command::operator<(const Command& c) {
    return c.getTime() > getTime();
}

void Command::commandPushBack(int p) {
    parametre.push_back(p);
}
