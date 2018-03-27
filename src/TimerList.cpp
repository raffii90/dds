﻿/*
   DDS, a bridge double dummy solver.

   Copyright (C) 2006-2014 by Bo Haglund /
   2014-2018 by Bo Haglund & Soren Hein.

   See LICENSE and README.
*/


/*
   See TimerList.h for some description.
*/

#include <sstream>

#include "dds.h"
#include "TimerList.h"

#define DDS_TIMERS 50


TimerList::TimerList()
{
  TimerList::Reset();
}


TimerList::~TimerList()
{
}


void TimerList::Reset()
{
  fname = "";

  timerGroups.resize(TIMER_GROUPS);

  timerGroups[TIMER_NO_AB].SetNames("AB");
  timerGroups[TIMER_NO_MAKE].SetNames("Make");
  timerGroups[TIMER_NO_UNDO].SetNames("Undo");
  timerGroups[TIMER_NO_EVALUATE].SetNames("Evaluate");
  timerGroups[TIMER_NO_NEXTMOVE].SetNames("NextMove");
  timerGroups[TIMER_NO_QT].SetNames("QuickTricks");
  timerGroups[TIMER_NO_LT].SetNames("LaterTricks");
  timerGroups[TIMER_NO_MOVEGEN].SetNames("MoveGen");
  timerGroups[TIMER_NO_LOOKUP].SetNames("Lookup");
  timerGroups[TIMER_NO_BUILD].SetNames("Build");
}


void TimerList::SetFile(const string& fnameIn)
{
  fname = fnameIn;
}


void TimerList::Start(
  const unsigned groupno,
  const unsigned timerno)
{
  if (groupno >= TIMER_GROUPS)
    return;
  timerGroups[groupno].Start(timerno);
}


void TimerList::End(
  const unsigned groupno,
  const unsigned timerno)
{
  if (groupno >= TIMER_GROUPS)
    return;
  timerGroups[groupno].End(timerno);
}


bool TimerList::Used() const
{
  for (unsigned g = 0; g < TIMER_GROUPS; g++)
  {
    if (timerGroups[g].Used())
      return true;
  }
  return false;
}


void TimerList::PrintStats() const
{
  if (! TimerList::Used())
    return;

  // Approximate the exclusive times of each function.
  // The ABsearch*() functions are recursively nested,
  // so subtract out the one below.
  // The other ones are subtracted out based on knowledge
  // of the functions.

  TimerGroup ABGroup;
  ABGroup = timerGroups[0];
  ABGroup.Differentiate();
  for (unsigned g = 1; g < TIMER_GROUPS; g++)
    ABGroup -= timerGroups[g];

  Timer ABTotal;
  ABGroup.SetNames("AB");
  ABGroup.Sum(ABTotal);
  ABTotal.SetName("Sum");

  Timer sumTotal = ABTotal;
  for (unsigned g = 1; g < TIMER_GROUPS; g++)
  {
    Timer t;
    timerGroups[g].Sum(t);
    sumTotal += t;
  }

  ofstream fout;
  fout.open(fname);

  fout << timerGroups[0].Header();
  fout << ABGroup.SumLine(sumTotal);
  for (unsigned g = 1; g < TIMER_GROUPS; g++)
    fout << timerGroups[g].SumLine(sumTotal);
  fout << timerGroups[0].DashLine();
  fout << sumTotal.SumLine(sumTotal) << endl;

  if (ABGroup.Used())
  {
    fout << ABGroup.Header();
    fout << ABGroup.TimerLines(ABTotal);
    fout << ABGroup.DashLine();
    fout << ABTotal.SumLine(ABTotal) << endl;
  }

#ifdef DDS_TIMING_DETAILS
  fout << timerGroups[0].DetailHeader();
  for (unsigned g = 0; g < TIMER_GROUPS; g++)
    fout << timerGroups[g].DetailLines();
  fout << endl;
#endif
  
  fout.close();
}

