#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

typedef vector<vector<unsigned char>> mapa_type;

// Función auxiliar de VisualizaPlan
void AnularMatriz(mapa_type & matriz);

// Funciones auxiliares generales
bool CasillaTransitable(const ubicacion &  casilla,
                        const mapa_type & mapa);

ubicacion NextCasilla(const ubicacion & ub);

bool VeoSonambulo(const stateN0 & st);

// Nivel 0
stateN0 apply(const Action & a, const stateN0 & st, const mapa_type & mapa);

bool Find(const stateN0 & item, const list<nodeN0> & lista);

list<Action> AnchuraSoloJugador (const stateN0 & inicio, const ubicacion & final,
                                 const mapa_type & mapa);

// Nivel 1
stateN1 apply(const Action & a, const stateN1 & st, const mapa_type & mapa);

list<Action> AnchuraSonambulo (const stateN1 & inicio, const ubicacion & final,
                               const mapa_type & mapa);


// Función para mostrar el mapa en el entorno gráfico
void ComportamientoJugador::VisualizaPlan(const stateN0 & st, const list<Action> & plan) {
  AnularMatriz(mapaConPlan);
  stateN0 cst = st;

  auto it = plan.begin();
  while (it != plan.end()) {
    switch (*it) {
    case actFORWARD:
      cst.jugador = NextCasilla(cst.jugador);
      mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
      break;
    case actTURN_R:
      cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 2) % 8);
      break;
    case actTURN_L:
      cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
      break;
    case actSON_FORWARD:
      cst.sonambulo = NextCasilla(cst.sonambulo);
      mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
      break;
    case actSON_TURN_SR:
      cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 1) % 8);
      break;
    case actSON_TURN_SL:
      cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 7) % 8);
      break;
    default: break;
    }
    it++;
  }
}

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
  if (sensores.nivel == 4)
    return accion;

  if (!hayPlan) {
    c_state.jugador.f = sensores.posF;
    c_state.jugador.c = sensores.posC;
    c_state.jugador.brujula = sensores.sentido;
    c_state.sonambulo.f = sensores.SONposF;
    c_state.sonambulo.c = sensores.SONposC;
    c_state.sonambulo.brujula = sensores.SONsentido;
    goal.f = sensores.destinoF;
    goal.c = sensores.destinoC;
    switch (sensores.nivel) {
      case 0: {
        stateN0 c_stateN0(c_state);
        plan = AnchuraSoloJugador(c_stateN0, goal, mapaResultado);
        break;
      }
      case 1: {
        stateN1 c_stateN1(c_state);
        plan = AnchuraSonambulo(c_stateN1, goal, mapaResultado);
        break;
      }
      case 2: {
        // plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
        break;
      }
      case 3: {
        // plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
        break;
      }
    }

    if (plan.size() > 0) {
      VisualizaPlan(c_state, plan);
      hayPlan = true;
    }
  }
  if (hayPlan && plan.size() > 0) {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size() == 0) {
    hayPlan = false;
  }

	return accion;
}


int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Función auxiliar de VisualizaPlan
void AnularMatriz(mapa_type & matriz) {
  for (int i = 0; i < matriz.size(); i++)
    for (int j = 0; j < matriz[0].size(); j++)
      matriz[i][j] = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Funciones auxiliares generales

bool CasillaTransitable(const ubicacion &  casilla,
                        const mapa_type & mapa) {
  return (mapa[casilla.f][casilla.c] != 'P'
          && mapa[casilla.f][casilla.c] != 'M');
}

ubicacion NextCasilla(const ubicacion & ub) {
  ubicacion next = ub;
  switch (ub.brujula) {
    case norte:
      next.f = ub.f - 1;
      break;
    case noreste:
      next.f = ub.f - 1;
      next.c = ub.c + 1;
      break;
    case este:
      next.c = ub.c + 1;
      break;
    case sureste:
      next.f = ub.f + 1;
      next.c = ub.c + 1;
      break;
    case sur:
      next.f = ub.f + 1;
      break;
    case suroeste:
      next.f = ub.f + 1;
      next.c = ub.c - 1;
      break;
    case oeste:
      next.c = ub.c - 1;
      break;
    case noroeste:
      next.f = ub.f - 1;
      next.c = ub.c - 1;
      break;
    default: break;
  }

  return next;
}



/*****************************************************************************
                                Nivel 0
*****************************************************************************/

stateN0 apply(const Action & a, const stateN0 & st, const mapa_type & mapa) {
  stateN0 st_result = st;
  ubicacion sig_ubicacion;

  switch (a) {
  case actFORWARD:
    sig_ubicacion = NextCasilla(st.jugador);
    if (CasillaTransitable(sig_ubicacion, mapa) && 
        !(sig_ubicacion.f == st.sonambulo.f && sig_ubicacion.c == st.sonambulo.c)) {
      st_result.jugador = sig_ubicacion;
    }
    break;
  case actTURN_L:
    st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6) % 8);
    break;
  case actTURN_R:
    st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2) % 8);
    break;
  default: break;
  }
  return st_result;
}

bool Find(const stateN0 & item, const list<nodeN0> & lista) {
  auto it = lista.begin();
  while (it != lista.end() && !((*it).st == item))
    it++;
  
  return (!(it == lista.end()));
}

list<Action> AnchuraSoloJugador (const stateN0 & inicio, const ubicacion & final,
                         const mapa_type & mapa) {
  nodeN0 current_node;
  list<nodeN0> frontier;
  set<nodeN0> explored;
  list<Action> plan;
  current_node.st = inicio;
  bool SolutionFound = (current_node.st.jugador.f == final.f
                        && current_node.st.jugador.c == final.c);
  frontier.push_back(current_node);

  while (!frontier.empty() && !SolutionFound) {
    frontier.pop_front();
    explored.insert(current_node);

    // Generar hijo actFORWARD
    nodeN0 child_forward = current_node;
    child_forward.st = apply(actFORWARD, current_node.st, mapa);
    if (child_forward.st.jugador.f == final.f 
      && child_forward.st.jugador.c == final.c) {
      child_forward.secuencia.push_back(actFORWARD);
      current_node = child_forward;
      SolutionFound = true;
    }
    else if (explored.find(child_forward) == explored.end()) {
      child_forward.secuencia.push_back(actFORWARD);
      frontier.push_back(child_forward);
    }

    if (!SolutionFound) {
      // Generar hijo actTURN_L
      nodeN0 child_turnl = current_node;
      child_turnl.st = apply(actTURN_L, current_node.st, mapa);
      if (explored.find(child_turnl) == explored.end()) {
        child_turnl.secuencia.push_back(actTURN_L);
        frontier.push_back(child_turnl);
      }
      // Generar hijo actTURN_R
      nodeN0 child_turnr = current_node;
      child_turnr.st = apply(actTURN_R, current_node.st, mapa);
      if (explored.find(child_turnr) == explored.end()) {
        child_turnr.secuencia.push_back(actTURN_R);
        frontier.push_back(child_turnr);
      }
    }

    if (!SolutionFound && !frontier.empty()) {
      current_node = frontier.front();
      while (!frontier.empty() && explored.find(current_node) != explored.end()) {
        frontier.pop_front();
        if (!frontier.empty())
          current_node = frontier.front();
      }
    }
  }

  if (SolutionFound)
    plan = current_node.secuencia;

  return plan;
}



/*****************************************************************************
                                Nivel 1
*****************************************************************************/

stateN1 apply(const Action & a, const stateN1 & st, const mapa_type & mapa) {
  stateN1 st_result = st;
  ubicacion sig_ubicacion;

  switch (a) {
  // Movimientos del jugador
  case actFORWARD:
    sig_ubicacion = NextCasilla(st.jugador);
    if (CasillaTransitable(sig_ubicacion, mapa) && 
        !(sig_ubicacion.f == st.sonambulo.f && sig_ubicacion.c == st.sonambulo.c)) {
      st_result.jugador = sig_ubicacion;
    }
    break;
  case actTURN_L:
    st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6) % 8);
    break;
  case actTURN_R:
    st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2) % 8);
    break;
  // Movimientos del sonambulo
  case actSON_FORWARD:
    sig_ubicacion = NextCasilla(st.sonambulo);
    if (CasillaTransitable(sig_ubicacion, mapa) && 
        !(sig_ubicacion.f == st.jugador.f && sig_ubicacion.c == st.jugador.c)) {
      st_result.sonambulo = sig_ubicacion;
    }
    break;
  case actSON_TURN_SL:
    st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 7) % 8);
    break;
  case actSON_TURN_SR:
    st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 1) % 8);
    break;
  default: break;
  }
  return st_result;
}


typedef pair<int,int> p;

// Dirección norte
vector<pair<int, int>> aux1 = {
	p(-1,-1), p(-1, 0), p(-1, 1), p(-2,-2), p(-2,-1),
	p(-2, 0), p(-2, 1), p(-2, 2), p(-3,-3), p(-3,-2),
	p(-3,-1), p(-3, 0), p(-3, 1), p(-3, 2), p(-3, 3)
};

// Dirección noroeste
vector<pair<int,int>> aux2 = {
	p( 0,-1), p(-1,-1), p(-1, 0), p( 0,-2), p(-1,-2), 
	p(-2,-2), p(-2,-1), p(-2, 0), p( 0,-3), p(-1,-3), 
	p(-2,-3), p(-3,-3), p(-3,-2), p(-3,-1), p(-3, 0)
};

bool VeoSonambulo(const stateN0 & st) {
  ubicacion jug = st.jugador;
  ubicacion son = st.sonambulo;
  bool visible = false;

  // La distancia máxima en línea recta que puede ver es
  // como mucho 3
  if (abs(son.f - jug.f) > 3 || abs(son.c - jug.c) > 3)
    return false;
  
  // Si está detrás, no lo va a poder ver
  if ((jug.brujula == norte || jug.brujula == noreste 
    || jug.brujula == noroeste) && son.f > jug.f)
    return false;
  if ((jug.brujula == sur || jug.brujula == sureste 
    || jug.brujula == suroeste) && son.f < jug.f)
    return false;
  if ((jug.brujula == este || jug.brujula == noreste 
    || jug.brujula == sureste) && son.c < jug.c)
    return false;
  if ((jug.brujula == oeste || jug.brujula == noroeste 
    || jug.brujula == suroeste) && son.c > jug.c)
    return false;

  set<pair<int,int>> posiciones;

  switch (jug.brujula)
	{
  case norte:
    for (auto i : aux1)
      posiciones.insert(pair<int,int>(jug.f + i.first, jug.c + i.second));
    break;
  case noreste:
    for (auto i : aux2)
      posiciones.insert(pair<int,int>(jug.f + i.second, jug.c - i.first));
    break;
  case este:
    for (auto i : aux1)
      posiciones.insert(pair<int,int>(jug.f + i.second, jug.c - i.first));
    break;
  case sureste:
    for (auto i : aux2)
      posiciones.insert(pair<int,int>(jug.f - i.first, jug.c - i.second));
    break;
  case sur:
    for (auto i : aux1)
      posiciones.insert(pair<int,int>(jug.f - i.first, jug.c - i.second));
    break;
  case suroeste:
    for (auto i : aux2)
      posiciones.insert(pair<int,int>(jug.f - i.second, jug.c + i.first));
    break;
  case oeste:
    for (auto i : aux1)
      posiciones.insert(pair<int,int>(jug.f - i.second, jug.c + i.first));
    break;
  case noroeste:	
    for (auto i : aux2)
      posiciones.insert(pair<int,int>(jug.f + i.first, jug.c + i.second));
    break;
	}

  if (!(posiciones.find(pair<int,int>(son.f, son.c)) == posiciones.end()))
    visible = true;

  return visible;
}


list<Action> AnchuraSonambulo (const stateN1 & inicio, const ubicacion & final,
                               const mapa_type & mapa) {
  nodeN1 current_node;
  list<nodeN1> frontier;
  set<nodeN1> explored;
  list<Action> plan;
  current_node.st = inicio;
  bool SolutionFound = (current_node.st.sonambulo.f == final.f
                        && current_node.st.sonambulo.c == final.c);
  frontier.push_back(current_node);

  while (!frontier.empty() && !SolutionFound) {
    frontier.pop_front();
    explored.insert(current_node);

    // Solo puede dar instrucciones al sonambulo si lo ve
    if (VeoSonambulo(current_node.st)) {
      // Generar hijo actSON_FORWARD
      nodeN1 child_forward = current_node;
      child_forward.st = apply(actSON_FORWARD, current_node.st, mapa);
      if (child_forward.st.sonambulo.f == final.f 
        && child_forward.st.sonambulo.c == final.c) {
        child_forward.secuencia.push_back(actSON_FORWARD);
        current_node = child_forward;
        SolutionFound = true;
      }
      else if (explored.find(child_forward) == explored.end()) {
        child_forward.secuencia.push_back(actSON_FORWARD);
        frontier.push_back(child_forward);
      }

      if (!SolutionFound) {
        // Generar hijo actSON_TURN_SL
        nodeN1 child_turnsl = current_node;
        child_turnsl.st = apply(actSON_TURN_SL, current_node.st, mapa);
        if (explored.find(child_turnsl) == explored.end()) {
          child_turnsl.secuencia.push_back(actSON_TURN_SL);
          frontier.push_back(child_turnsl);
        }
        // Generar hijo actSON_TURN_SR
        nodeN1 child_turnsr = current_node;
        child_turnsr.st = apply(actSON_TURN_SR, current_node.st, mapa);
        if (explored.find(child_turnsr) == explored.end()) {
          child_turnsr.secuencia.push_back(actSON_TURN_SR);
          frontier.push_back(child_turnsr);
        }
      }
    }
    // else if (!SolutionFound) {
    else {
      // Generar hijo actFORWARD
      nodeN1 child_forward = current_node;
      child_forward.st = apply(actFORWARD, current_node.st, mapa);
      if (explored.find(child_forward) == explored.end()) {
          // && !(VeoSonambulo(current_node.st) && !VeoSonambulo(child_forward.st))) {
        child_forward.secuencia.push_back(actFORWARD);
        frontier.push_back(child_forward);
      }

      // Generar hijo actTURN_L
      nodeN1 child_turnl = current_node;
      child_turnl.st = apply(actTURN_L, current_node.st, mapa);
      if (explored.find(child_turnl) == explored.end()) {
          // && !(VeoSonambulo(current_node.st) && !VeoSonambulo(child_turnl.st))) {
        child_turnl.secuencia.push_back(actTURN_L);
        frontier.push_back(child_turnl);
      }
      // Generar hijo actTURN_R
      nodeN1 child_turnr = current_node;
      child_turnr.st = apply(actTURN_R, current_node.st, mapa);
      if (explored.find(child_turnr) == explored.end()) {
          // && !(VeoSonambulo(current_node.st) && !VeoSonambulo(child_turnr.st))) {
        child_turnr.secuencia.push_back(actTURN_R);
        frontier.push_back(child_turnr);
      }
    }

    // Si no se ha encontrado todavía la solución y quedan nodos en frontier
    // seguir explorando
    if (!SolutionFound && !frontier.empty()) {
      current_node = frontier.front();
      while (!frontier.empty() && explored.find(current_node) != explored.end()) {
        frontier.pop_front();
        if (!frontier.empty())
          current_node = frontier.front();
      }
    }
  }

  if (SolutionFound)
    plan = current_node.secuencia;

  return plan;
}
