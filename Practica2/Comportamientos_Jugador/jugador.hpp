#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <queue>

struct stateN0 {
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator == (const stateN0 & otro) const {
    return (jugador == otro.jugador 
    && sonambulo.f == otro.sonambulo.f && sonambulo.c == otro.sonambulo.c);
  }
};

struct nodeN0 {
  stateN0 st;
  list<Action> secuencia;

  bool operator == (const nodeN0 & n) const {
    return (st == n.st);
  }

  bool operator < (const nodeN0 & n) const {
    if (st.jugador.f < n.st.jugador.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c < n.st.jugador.c)
      return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c == n.st.jugador.c 
             && st.jugador.brujula < n.st.jugador.brujula)
      return true;
    else
      return false;
  }
};

struct stateN1 : stateN0 {
  stateN1(){}
  stateN1(stateN0 st0) {
    jugador = st0.jugador;
    sonambulo = st0.sonambulo;
  }
  // Sobrecarga del operador
  bool operator == (const stateN1 & otro) const {
    return (jugador == otro.jugador && sonambulo == otro.sonambulo);
  }
};

struct nodeN1 {
  stateN1 st;
  list<Action> secuencia;

  bool operator == (const nodeN1 & n) const {
    return (st == n.st);
  }

  bool operator < (const nodeN1 & n) const {
    if (st.jugador.f < n.st.jugador.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c < n.st.jugador.c)
      return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c == n.st.jugador.c 
             && st.jugador.brujula < n.st.jugador.brujula)
      return true;
    else if (st.jugador == n.st.jugador && st.sonambulo.f < n.st.sonambulo.f)
      return true;
    else if (st.jugador == n.st.jugador && st.sonambulo.f == n.st.sonambulo.f && st.sonambulo.c < n.st.sonambulo.c)
      return true;
    else if (st.jugador == n.st.jugador && st.sonambulo.f == n.st.sonambulo.f && st.sonambulo.c == n.st.sonambulo.c 
             && st.sonambulo.brujula < n.st.sonambulo.brujula)
      return true;
    else
      return false;
  }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const stateN0 & st, const list<Action> & plan);


  private:
    // Declarar Variables de Estado
    list<Action> plan;
    bool hayPlan;
    stateN0 c_state;
    ubicacion goal;

};

#endif
