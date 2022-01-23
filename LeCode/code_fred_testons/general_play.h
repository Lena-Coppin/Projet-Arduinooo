/*********************************************************************
 *          On retrouve ici un regroupement de code
 *           provenant des deux pianos, permettant
 *            ainsi le fonctionnement général 
 *              des touches du variophuino.
 *               (effets bluetooth inclus)
*********************************************************************/



/*
#include <SoftwareSerial.h>
#define rx_bb8 14
#define tx_bb8 15
SoftwareSerial BlueBlue8(rx_bb8, tx_bb8);
*/
//Inutile quand on se sert du téléphone pour "bluetoother"



void basic_updater() {
  int i = quelles_3_touches_gauche();
  int a;
  mineur_ou_majeur();
  if (minl) {a = 1;}
  if (majl) {a = 2;}
  left_index[0] = i;
  left_index[1] = i+a;
  left_index[2] = i+3;
  actual_frequencies_update();
};


bool left_all_ok() {
  left_t_update();
  return (tl_played && correct_left_playing());
};

bool right_all_ok() {
  right_t_update();
  return (tr_played && correct_right_playing());
};


bool skippable(char a) {
  //Pour les commandes dont l'exécution est trop longue
  return (a=='S')||(a=='V');
};



void super_3_times_y_updater(int y=4, int deltime=50) {
  //boucle for: 3 fois y fois 'deltime' millisecondes
  for (int i = 0; i < 3; i++) {
    basic_updater();
    commands();
    if (skippable(charblu) || mode_is_changing) {break;}
    
    if (left_all_ok()) {left_tone.play(mini_l_pad[left_index[i]], y*deltime);}
    else {
      left_tone.stop();
      break;
    }
    
    if (right_all_ok()) {right_play(1000);}
    else {right_tone.stop();}

    //boucle for: y fois 'deltime' millisecondes
    for (int j = 0; j < y; j++) {
      delay(deltime);
      basic_updater();
      commands();
      if (skippable(charblu) || mode_is_changing) {break;}
      
      if (!left_all_ok()) {
        left_tone.stop();
        break;
      }
      
      if (right_all_ok()) {right_play(1000);}
      else {right_tone.stop();}
    }
    
    if (!left_all_ok()) {
        left_tone.stop();
        break;
    }
  }
};




/*========================[PARTIE EFFETS]========================*/



//  effet: aucun                    ->      mode = 0
void normal_mode(int laps) {
  super_3_times_y_updater(4, laps);
  basic_updater();
  if (right_all_ok()) {right_play(1000);}
  else {right_tone.stop();}
};



//  effet: arpège                   ->      mode = 1
int quelles_special_3_touches_droite() {
  int i;
  if (tr1_correctly_played) {i = 0;}
  else if (tr2_correctly_played) {i = 1;}
  else if (tr3_correctly_played) {i = 2;}
  else if (tr4_correctly_played) {i = 3;}
  else if (tr5_correctly_played) {i = 4;}
  else if (tr6_correctly_played) {i = 5;}
  else if (tr7_correctly_played) {i = 6;}
  else if (tr8_correctly_played) {i = 7;}
  else if (tr9_correctly_played) {i = 8;}
  else if (tr10_correctly_played) {i = 9;}
  else if (tr11_correctly_played) {i = 10;}
  else if (tr12_correctly_played) {i = 11;}
  else if (tr13_correctly_played) {i = 12;}
  return i;
};

void special_arpege_updater() {
  int i = quelles_special_3_touches_droite();
  int a;
  mineur_ou_majeur();
  if (minl) {a = 3;}
  if (majl) {a = 4;}
  special_right_index[0] = i;
  special_right_index[1] = i + a;
  special_right_index[2] = i + 7;
  actual_frequencies_update();
};

void left_special_arpege_play(int t=1000) {
  int i;
  if (tl1_correctly_played) {i = 0;}
  else if (tl2_correctly_played) {i = 2;}
  else if (tl3_correctly_played) {i = 4;}
  else if (tl4_correctly_played) {i = 5;}
  else if (tl5_correctly_played) {i = 7;}
  else if (tl6_correctly_played) {i = 9;}
  else if (tl7_correctly_played) {i = 11;}
  left_tone.play(notes[36+i], t);
};

void arpege_mode(int y=4, int deltime=50) {
  for (int i = 0; i < 3; i++) {
    special_arpege_updater();
    commands();
    if (skippable(charblu) || mode_is_changing) {break;}
    if (right_all_ok()) {right_tone.play(special_actual_r_frequencies[special_right_index[i]], 200);}
    else {
      right_tone.stop();
      break;
    }
    if (left_all_ok()) {left_special_arpege_play(1000);}
    else {left_tone.stop();}
    for (int j = 0; j < y; j++) {
      delay(3);
      special_arpege_updater();
      commands();
      if (skippable(charblu) || mode_is_changing) {break;}
      if (!right_all_ok()) {
        right_tone.stop();
        break;
      }
      if (left_all_ok()) {left_special_arpege_play(1000);}
      else {left_tone.stop();}
    }
    if (!right_all_ok()) {
        right_tone.stop();
        break;
    }
  }
  special_arpege_updater();
  if (left_all_ok()) {left_special_arpege_play(1000);}
  else {left_tone.stop();}
};



/*================================================================*/




void pianos_checking(int laps=50) {
  if (laps < 3) {laps = 3;}
  else if (laps > 250) {laps = 250;}
  commands();
  
  if (mode == 1) {special_arpege_updater();}
  else {basic_updater();}
  
  left_t_update();
  right_t_update();
  mode_is_changing = LOW;
  while (tl_played || tr_played) {
    if (mode == 1) {special_arpege_updater();}
    else {basic_updater();}
    
    if (correct_left_playing() || correct_right_playing()) {
      
      if (mode == 1) {arpege_mode(4, laps);}
      else {normal_mode(laps);}
      
    }
    left_t_update();
    right_t_update();
    commands();
    charblu = '\0';
    mode_is_changing = LOW;
  }
  left_tone.stop();
  right_tone.stop();
};
