/*
    Karl Toby Rosenberg
    knapsack 0/1 implementation
*/

#include <stdio.h>
#include <stdlib.h>
//#include <inttypes.h>

long** knapsack_0_1(long num_items, long max_weight, long* values, long* weights);
long* make_items_list(long num_items, long max_weight, long* weights, long** s_table);
int display_items(long* items_list, long* values);
int print_solutions_table(long num_items, long max_weight, long** s_table);
