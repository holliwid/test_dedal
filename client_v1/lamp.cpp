#include "lamp.h"

Lamp::Lamp(int id, bool isOn)
    : m_id(id), m_isOn(isOn)
{
}

int Lamp::getId() const {
    return m_id;
}


bool Lamp::isOn() const {
    return m_isOn;
}
