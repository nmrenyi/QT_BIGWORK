#ifndef COMMAND_H
#define COMMAND_H
#include <vector>
#include <string>

class Command
{
private:
    std::vector<int> parametre;
    std::string commandName;
    int time;

public:
    int timeFinish;
    Command();
    void setTime(int);
    std::vector<int> getPara();
    void setName(std::string);
    int getTime() const;
    std::string getName();
    bool operator<(const Command&);
    void commandPushBack(int);
};


#endif // COMMAND_H
