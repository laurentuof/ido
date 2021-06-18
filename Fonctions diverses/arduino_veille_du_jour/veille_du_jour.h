
//**********************************************************************************************
void veille_de(int *jj, int *mm, int *aa) {
  //
  //    Rédacteur : Laurent MARCHAL
  //    Date      : juin 2021
  //    Version   : 1.0
  //
  //
  //  Utilité : permettre de calculer la veille du jour pour un système simple
  //  (ex. arduino) qui ne possède pas de fonctions de date/heure avancées.
  //
  //
  //  *jj : pointe vers un int représentant le jour du mois
  //  *mm : pointe vers un int représentant le mois de l'année
  //  *aa : pointe vers un int représentant l'année
  //
  //  La fonction calcule la date du jour précédent (la veille)
  //  et modifie les 3 valeurs (jj, mm et aa) en conséquence
  //
  //  Prend en compte les mois à 28, 29, 30 et 31 jours
  //
  //  
  *jj = *jj-1;
  if (*jj == 0) {
    *jj=31; // pour le moment, avant de avois si c'est un mois à 28, 29, 30 ou 31jours 
    *mm=*mm-1;
    if (*mm == 0) {
      *mm=12;
      *aa = *aa -1;
    }
    if (*mm==4 or *mm == 6 or *mm == 9 or *mm == 11) {
      *jj = 30;
    }
    if (*mm == 2) {
      if ((*aa % 4) == 0) {
        *jj=29;
      } else {
        *jj=28;
      }
    }
  }
}
