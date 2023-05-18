#ifndef LAMP_H
#define LAMP_H

#include <QString>

class Lamp {
public:
    Lamp(int id = 0, bool isOn = false);

    int getId() const;
    bool isOn() const;

private:
    int m_id;
    bool m_isOn;
};

#endif // LAMP_H
