//cmake -DCMAKE_BUILD_TYPE=Release

# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}
void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{
   // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const{
    //el numero de mi jugador actual.
    int player = actual -> getCurrentPlayerId();
    
    //Vector que almacena los dados que se pueden usar para el movimiento.
    vector<int> current_dices;

    //Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color,int>> current_pieces;

    /*Obtengo el vector de dados que puedo usar para el movimiento
    * actual-> getvaiableNormalDices(player) --> solo dados disponibles en ese turno.
    * Importante: "getAvailableNormalDices" me da los dados que puedo usar en el turno actual.
    * Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    * Puedo saber que más dados tengo, aunque no los pueda usar en este turno, con:
    * actual -> getNoramlDices(player) --> todos los dados
    */

    current_dices = actual->getAvailableNormalDices(player);

    //En vez de elegir un dado al azar, miro primero cuáles  tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;
    for(int i = 0; i < current_dices.size();i++){
        //Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player,current_dices[i]);
        
        //Si se pueden mover fichcas para el dado actual, lo añado al vector de dados que pueden mover fichas
        if(current_pieces.size() > 0){
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }
    //Si no tengo ningún dado que pueda mover fichas, paso turno con un dado al azar(la macro SKIP_TURN me permite no mover)
    if(current_dices_que_pueden_mover_ficha.size() == 0){
        dice = current_dices[rand() % current_dices.size()];

        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); //LE tengo que indicar mi color actual al pasar turno
    }

    //en caso contrario, elijo un dado de forma aleatoria entre los que pueden mover ficha
    else{
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];

        //Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual->getAvailablePieces(player,dice);

        //Muevo la ficha al azar de entre las que se pueden mover
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color & c_piece, int & id_piece, int & dice) const{
    //Elijo el dado haciendo lo mismo que el jugador anterior
    thinkAleatorioMasInteligente(c_piece,id_piece,dice);
    /**
     * Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar
     * Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar el dado especial a)
     * la que esté más adelante (equivalentemente, la más cercana a la meta)
    */
   int player = actual->getCurrentPlayerId();
   vector<tuple<color,int>> current_pieces = actual->getAvailablePieces(player,dice);
   int id_ficha_mas_adelantada = -1;
   color col_ficha_mas_adelantada = none;
   int min_distancia_meta = 9999;
   for(int i = 0;i < current_pieces.size(); i++){
       //distanceToGoal(color,id) devuelve la distancia a la meta de la ficha [id] del
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col,id);
        if(distancia_meta < min_distancia_meta){
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
   }

   //SI no he encontrado ninguna ficha, paso turno.
   if(id_ficha_mas_adelantada == -1){
       id_piece = SKIP_TURN;
       c_piece = actual->getCurrentColor(); //LE tengo que indicar mi color actual al pasar turno.
   }
   
   //En caso contrario, moveré la ficha más adelantada
   else{
       id_piece = id_ficha_mas_adelantada;
       c_piece = col_ficha_mas_adelantada;
   }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice)const{
    /**
     * Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual
     * Cuando ya he recorrido todos los hijos, la función devuelve el es tado actual. De esta forma puedo saber cuándo paro de iterar.
     * 
     * Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren()
     * Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama
    */
   ParchisBros hijos = actual->getChildren();
   bool me_quedo_con_esta_accion = false;

   /**
    * La clase ParchisBRos viene con un iterador muy util y sencillo de usar
    * Al hacer begin() accedemos al primer hijo de la rama, y cada vez que hagamos ++it saltaremos al siguiente hijo.
    * Comparando con el iterador end() podemos consultar cuando hemos terminado de visitar los hijos.
    * 
    * Voy a moverme a la casilla siempre con la que gane más energía, salvo que me encuentre con algún movimiento interesante, como comer fichas o llegar a la meta
   */
  int current_power = actual->getPowerBar(this->jugador).getPower();
  int max_power = -101; //máxima ganancia de energía

  for(ParchisBros::Iterator it = hijos.begin(); it != hijos.end() && !me_quedo_con_esta_accion; ++it){
      Parchis siguiente_hijo = *it;
      /**
       * Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa accion
       * Termino el bucle en otro caso
      */
     if(siguiente_hijo.isEatingMove() or 
        siguiente_hijo.isGoalMove() or
        (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador)){
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
     }

     //En caso contrario, me voy quedando con el que más energía me da
     else{
         int new_power = siguiente_hijo.getPower(this->jugador);
         if(new_power - current_power > max_power){
             c_piece = it.getMovedColor();
             id_piece = it.getMovedPieceId();
             dice = it.getMovedDiceValue();
             max_power = new_power - current_power;
         }
     }
     /**
      * SI he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia
      * (Ya lo he hechjo antes, cuando les he asignado valor con el iterador)
     */
  }
}

double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const{
    //ParchisBros hijos = actual.getChildren();
    color c_piece_aux = none;
    int id_piece_aux = -1;
    int dice_aux = -1;
    if(profundidad == profundidad_max || actual.gameOver()){
        return heuristic(actual,jugador);
    }
    if(jugador == actual.getCurrentPlayerId()){
        //double max_eval = alpha;
        ParchisBros hijos = actual.getChildren();
        for(auto it = hijos.begin(); it != hijos.end(); ++it){
            if(profundidad == 0){
               cout << it.getMovedColor() << " " << it.getMovedPieceId() << " " << it.getMovedDiceValue() << endl;
                cout << "valor alpha = " << alpha << "y beta = " << beta << endl;   
            }
            Parchis siguienteHijo = *it;
            double eval = Poda_AlfaBeta(siguienteHijo,jugador,profundidad+1,profundidad_max,c_piece_aux,id_piece_aux,dice_aux,alpha,beta,heuristic);
            if(eval > alpha){
                alpha = eval;
                if(profundidad == 0){
                    c_piece = it.getMovedColor();
                    id_piece = it.getMovedPieceId();
                    dice = it.getMovedDiceValue();
                    cout << c_piece << " " << id_piece << " " << dice << "V= " << alpha << endl;
                }
                if(beta <= alpha) return alpha;
            }         
        }
        return alpha;
    }else{
        double min_eval = beta;
        ParchisBros hijos = actual.getChildren();
        for(auto it = hijos.begin() ; it != hijos.end() ; ++it){
            Parchis siguienteHijo = *it;
            double eval = Poda_AlfaBeta(siguienteHijo,jugador,profundidad+1,profundidad_max,c_piece_aux,id_piece_aux,dice_aux,alpha,beta,heuristic);
            min_eval = min(min_eval,eval);
            beta = min(beta,eval);
            if(beta <= alpha) return beta;
        }
        return beta;
    }
}
/*
NINJA 1: 
yo J1 = 0K
yo J2 = OK
NINJA 2:
yo J1 = OK
yo J2 =
*/
double AIPlayer::primeraHeuristica(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;
    if(ganador == jugador){
        return gana;
    }
    else if(ganador == oponente){
        return pierde;
    }
    else{
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);
        int puntuacionJugador = 0;
        int puntuacionOponente = 0;
        
        int mejorJugador = 0, mejorOponente = 0;

        vector<int> estadoJugador(2,0), estadoOponente(2,0);   //inicialización de 2 elementos, ambos a 0.
        for(int i = 0; i < my_colors.size(); i++){
            color c = my_colors[i];
            for(int j = 0; j < num_pieces;j++){
                Piece pieza = estado.getBoard().getPiece(c,j);
                Box casilla = pieza.get_box();
                int distanciaMeta = estado.distanceToGoal(c,j);
                estadoJugador[i] += (74-distanciaMeta);
            }
            if(estadoJugador[i] >= mejorJugador){
                mejorJugador = estadoJugador[i];
            }
        }
        for(int i = 0; i < op_colors.size(); i++){
            color c = op_colors[i];
            for(int j = 0 ; j < num_pieces ; j++){
                Piece pieza = estado.getBoard().getPiece(c,j);
                Box casilla = pieza.get_box();

                int distanciaMeta = estado.distanceToGoal(c,j);
                estadoOponente[i] += (74 - distanciaMeta);
            }
            if(estadoOponente[i] > mejorOponente){
                mejorOponente = estadoOponente[i];
            }
        }

        puntuacionJugador += estado.getAvailableNormalDices(jugador).size() * 6;
        for(int i = 0; i < my_colors.size() ; i++){
            color c = my_colors[i];
            //MIRAR TEMAS DE COMER OPONENTE       
            //MIRAR PODERES
            
            for(int j = 0; j < num_pieces;j++){
                const Piece pieza = estado.getBoard().getPiece(c,j);
                const Box casilla = pieza.get_box();

                if(casilla.type == home) puntuacionOponente += 60;
                else{
                    if(estado.isSafePiece(c,j)) puntuacionJugador += 50;
                    else if(casilla.type == final_queue) puntuacionJugador += 50;
                    else if(casilla.type == goal) puntuacionJugador += 150;
                    else if(estado.isEatingMove()) puntuacionJugador += 50;
                    else if(estado.eatenPiece().first == c) puntuacionJugador -= 100;
                    int distanciaMeta = estado.distanceToGoal(c,j);
                    puntuacionJugador += (74 - distanciaMeta);
                }
            }
        }

        puntuacionOponente += estado.getAvailableNormalDices(oponente).size() * 6;
        for(int i = 0; i < op_colors.size() ; i++){
            color c = op_colors[i];
            for(int j = 0; j < num_pieces;j++){
                const Piece pieza = estado.getBoard().getPiece(c,j);
                const Box casilla = pieza.get_box();
                if(casilla.type == home) puntuacionJugador += 60;
                else{
                    if(estado.isSafePiece(c,j)) puntuacionOponente += 50;
                    else if(casilla.type == final_queue) puntuacionOponente += 50;
                    else if(casilla.type == goal) puntuacionOponente += 150;
                    else if(estado.isEatingMove()) puntuacionOponente += 500;
                    else if(estado.eatenPiece().first == c) puntuacionOponente -= 100;
                    int distanciaMeta = estado.distanceToGoal(c,j);
                    puntuacionOponente += (74 - distanciaMeta);
                }
            }
        }
        return puntuacionJugador - puntuacionOponente;
    }
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    int jugador = actual->getCurrentPlayerId();


    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, 6, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
               cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, 6, c_piece, id_piece, dice, alpha, beta, primeraHeuristica);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

            break;
        case 3:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

            break;
    }
}


double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

