#ifndef MULTICONFIG_H
#define MULTICONFIG_H
#include "config.h"

suint PredictedByConjecture(const vector<Config>& configs, bool quiet = false);

Config FindBestComplete(const vector<Config>& L, uchar numrows);

list<Config> WhatIsMissing(vector<Config> L,
                             suint n = 0,
                             const Config& CustomComplete = Config(0, 0));

void FindForbExact(vector<Config> ForbConf, nuint m, nuint start);

list<Config> ReadFromFile(string filename);

list<Config> GoToHigherRows(const vector<Config>& WIM, suint n = 0);

void PrintAsWIM(const list<Config>& wim);

vector<Config> ConvertToVector(const list<Config>& someList);
list<Config> ConvertToList(const vector<Config>& someVec);

bool isValidUnderWIMConditions(const Column& D, const VSUI& F, const vector<Config>& WIM);

list<Config> Maximals(list<Config> hello);
list<Config> Minimals(list<Config> hello);

//list<Config> EquivalenceClass(const list<Config>& hello);

suint MaxColCount(const vector<Config>& L);

list<Config> EnterListOfConfigs();

vector<Config> InductiveChildren(const vector<Config>& ForbConf);

#endif // MULTICONFIG_H
