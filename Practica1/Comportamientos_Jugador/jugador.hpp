#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

struct state {
  int fil;
  int col;
  Orientacion brujula;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      estado_actual.fil = estado_actual.col = -1;
      estado_actual.brujula = norte;

      ultima_accion = actIDLE;
      primera_vez = true;

      tengoBikini = tengoZapatillas = false;

      bien_situado = false;
      girar_derecha = false;
      giro_grande = false;

      for (int i = 0; i < size; i++)
          mapaVisitado.push_back(vector<int>(size, 0));

      veces_giradas = 0;

      ultima_casilla = '\0';
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:
  
  // Declarar aquí las variables de estado
  state estado_actual;
  Action ultima_accion;
  vector<vector<int>> mapaVisitado;
  bool primera_vez;
  bool tengoBikini;
  bool tengoZapatillas;
  bool bien_situado;
  bool girar_derecha;
  bool giro_grande;
  int veces_giradas;
  unsigned char ultima_casilla;
};

#endif
