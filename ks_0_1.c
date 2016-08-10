/*
    Karl Toby Rosenberg
    knapsack 0/1 implementation
*/

#include "ks_0_1.h"

/*
    main
        creates a default set of items (values and weights arrays)
            and finds the solution to the knapsack problem
            applied to the items' values and weights,
            displays the solution as (chosen item index - chosen item value) pairs, one per line,
            and a final value total
        pass the number of items, the maximum weight, and all value weight (alternating) pairs
        to solve the problem using the data
        if there are no arguments, the program solves the problem with the default data
    
*/
int main(int argc, char** argv)
{
    long ret_val = 0;
    
    //initialize values and weights tables
    //the values and weights arrays are 1-indexed
    //for simpler indexing in the knapsack function
    long num_items = 0;
    long max_weight = 0;
    long* values = NULL;
    long* weights = NULL;

    //if valid (and valid number of) command line arguments passed
    //main, interpret argument 1 as the number of items, argument 2 as the maximum weight,
    //and the remaining arguments as value_i_1 weight_i_1 value_i_2 weight_i_2 ...
    if(argc != 1)
    {
        if(argc < 3)
        {
            printf("%s\n", "Invalid number of arguments");
            return -1;
        }

        char* end_ptr = NULL;
        errno = 0;

        //number of items
        num_items = strtol(argv[1], &end_ptr, 10);
        if(*end_ptr != 0 || errno != 0)
        {
            if(errno != 0)
            {
                perror(end_ptr);
                return errno;
            }
            else
            {
                printf("%s\n", "Invalid input");
                return -1;
            }
        }

        //check whether number of arguments is valid
        int vals_plus_wts_marker = 2*num_items + 3;
        if(argc != vals_plus_wts_marker)
        {
            printf("%s\n", "Invalid number of arguments");
            return -1;
        }

        //maximum weight
        max_weight = strtol(argv[2], &end_ptr, 10);
        if(*end_ptr != 0 || errno != 0)
        {
            if(errno != 0)
            {
                perror(end_ptr);
                return errno;
            }
            else
            {
                printf("%s\n", "Invalid input");
                return -1;
            }
        }

        //allocate memory for values and weights arrays
        values = malloc((num_items + 1)*sizeof(long));
        if(!values)return -1;
        weights = malloc((num_items + 1)*sizeof(long));
        if(!weights)
        {
            free(values);
            values = NULL;
            return -1;
        }

        long converted = 0;
        long i = 3;
        long offset = 2;
        values[0] = -1;
        weights[0] = 0;
        //transfer remaining command line arguments (val weight val weight...)
        //to values and weights arrays
        for(;i < vals_plus_wts_marker; i+= 2, ++offset)
        {
            converted = strtol(argv[i], &end_ptr, 10);
            if(*end_ptr != 0 || errno != 0)
            {
                free(values);
                values = NULL;
                free(weights);
                weights = NULL;

                if(errno != 0)
                {
                    perror(end_ptr);
                    return errno;
                }
                else
                {
                    printf("%s\n", "Invalid input");
                    return -1;
                }
            }
            values[i - offset] = converted;
            
            converted = strtol(argv[i + 1], &end_ptr, 10);
            if(*end_ptr != 0 || errno != 0)
            {
                free(values);
                values = NULL;
                free(weights);
                weights = NULL;

                if(errno != 0)
                {
                    perror(end_ptr);
                    return errno;
                }
                else
                {
                    printf("%s\n", "Invalid input");
                    return -1;
                }
            }
            weights[i - offset] = converted;
        }
    }
    //else use the default input
    else
    {

        num_items = 4;
        max_weight = 5;
        values = malloc((num_items + 1)*sizeof(long));
        if(!values)
        {
            return -1;
        }
        values[0] = -1;
        values[1] = 100;
        values[2] =  20;
        values[3] =  60;
        values[4] =  40;

        weights = malloc((num_items + 1)*sizeof(long));
        if(!weights)
        {
            free(values);
            values = NULL;
            return -1;
        }
        weights[0] = 0;
        weights[1] = 3;
        weights[2] = 2;
        weights[3] = 4;
        weights[4] = 1;
    }
    
    /*
    printf("n_i: %ld \n m_w: %ld \n", num_items, max_weight);
    long k = 0; 
    printf("%c", '{');
    for(; k < num_items; ++k)
    {
        printf("%ld, ", values[k]);
    } 
    printf("%ld}\n", weights[k]);

    printf("%c", '{');
    for(k = 0; k < num_items; ++k)
    {
        printf("%ld, ", weights[k]);
    }
    printf("%ld}\n", weights[k]);
    */


    //calculate the solutions and store in the solutions table
    long** solutions = knapsack_0_1(num_items, max_weight, values, weights);

    if(!solutions)
    {
        free(values);
        values = NULL;
        free(weights);
        weights = NULL;
        return -1;
    }
    
    //display the solutions table
    print_solutions_table(num_items, max_weight, solutions);

    //return an array of indices (indices of the items chosen in the solution)
    long* items_list = make_items_list(num_items, max_weight, weights, solutions);

    //if items_list is not created successfully, set an error value to return
    //at the end of main
    if(!items_list)
    {
        ret_val = -1;
    }
    //else display the chosen items as item index - item value pairs (one per line)
    else
    {
        display_items(items_list, values);
    }
    
    //free allocated memory
    int i = 0;
    for(; i <= num_items; ++i)
    {
        free(solutions[i]);
        solutions[i] = NULL;
    }
    free(solutions);
    solutions = NULL;
    free(values);
    values = NULL;
    free(weights);
    weights = NULL;
    free(items_list);
    items_list = NULL;
    
    return ret_val;       
}

/*
    knapsack_0_1
        solves the knapsack 0/1 problem 
    param:
        long num_items (number of items to consider)
        long max_weight (maximum weight to consider)
        long values[] (values of items)
        long weights[] (weights of items)
    return: 
        long** s_table (the solutions table, stores 
                        the maximum values that can be carried within
                        the given maximum weight)
        upon error returns NULL
*/
long** knapsack_0_1(long num_items, long max_weight, long* values, long* weights)
{
    if(num_items < 0 || max_weight < 0 || !values || !weights)return NULL;
    
    //s_table for "solutions table,
    //where dimension 1 is the number of items considered and
    //dimension 2 is the max weight considered"
    long** s_table = malloc((num_items + 1)*sizeof(long*));
    if(!s_table)return NULL;

    long itm = 1;
    long wt = 1;

    {
        //allocate memory for the first row,
        //initialize its values to 0 (no items considered)
        long* weights_i = calloc((max_weight + 1), sizeof(long));
        if(!weights_i)
        {
            free(s_table);
            return NULL;
        }
        else
        {
            s_table[0] = weights_i;
        }

        //allocate memory for the remainder of the table,
        for(; itm <= num_items; ++itm)
        {
            //allocate memory for a row for the current item
            weights_i = malloc((max_weight + 1)*sizeof(long));
            
            //if memory allocation unsuccessful,
            //free all successfully allocated memory preceding the unsuccessful attempt
            if(!weights_i)
            {
                while(--itm >= 0)
                {
                    free(s_table[itm]);
                }
                free(s_table);
                s_table = NULL;
                return NULL;
            }
            else
            {
                //initialize first column to 0 (maximum weight is 0)
                weights_i[0] = 0;
                //add the weights row to the solution table
                s_table[itm] = weights_i;
            }
        }
    }
 
    long alt_solution = 0;
    //increase the set of items to consider
    for(itm = 1; itm <= num_items; ++itm)
    {
        //increase the maximum carrying weight to consider
        for(wt = 1; wt <= max_weight; ++wt)
        {
            //since the current item has too large a weight,
            //use the solution from the set of items that does not
            //contain the current item
            if(weights[itm] > wt)
            {
                s_table[itm][wt] = s_table[itm-1][wt];
            }
            //otherwise, check whether it is better to add the new item or not
            else
            {
                //alt_solution, add the new item
                alt_solution = values[itm] + s_table[itm-1][wt - weights[itm]];
                //use the new item?
                if(alt_solution > s_table[itm-1][wt])
                {
                    s_table[itm][wt] = alt_solution;
                }
                //else do not add the new item
                else
                {
                    s_table[itm][wt] = s_table[itm-1][wt];
                }
            }
        }
    }

    return s_table;
}

/*
    make_items_list
        creates an array of longs (indices) referring to the items chosen in the solution
        to the knapsack problem
    param:
        long num_items (the number of items to consider) 
        long max_weight (the maximum weight to consider)
        long* weights (contains the weights associated with each item, 1-indexed)
        long** s_table (the solution table for the knapsack problem)
    return:
        upon success returns an array of longs (indices) referring to the chosen items,
            index 0 stores the number of items chosen
        upon error returns NULL
*/
long* make_items_list(long num_items, long max_weight, long* weights, long** s_table)
{
    if(num_items < 0 || max_weight < 0 || !s_table)return NULL;

    long* which_items = malloc((num_items + 1)*sizeof(long));
    if(!which_items)return NULL;

    long size_marker = 1;
    while(num_items > 0 && max_weight > 0)
    {
        //an item is part of the solution if it improves the total value of the
        //answer in the previous row (the maximum weight in the previous row
        //is the same, but the current item was not considered for the solution
        //in the previous row) 
        if(s_table[num_items][max_weight] != s_table[num_items-1][max_weight])
        {
            which_items[size_marker++] = num_items;
            max_weight -= weights[num_items];
        }
        --num_items;
    }
    //index 0 of which_items stores the number of items
    which_items[0] = size_marker;

    //minimize allocated memory for which_items 
    which_items = realloc(which_items, (size_marker)*sizeof(long));

    return which_items;
}

/*
    display_items
        displays the items and (optionally) their values
        that have been chosen in the solution to the knapsack problem
    param:
        long* items_list (the list of indices referring to each chosen item,
                         index 0 stores the number of items chosen in the solution)
        long* values (the values associated with each item, 1-indexed)
                     pass NULL if only item indices desired
    return:
        upon success returns 0 
        if nothing to print, returns -1
*/
int display_items(long* items_list, long* values)
{
    if(!items_list)return -1;
    
    int item = items_list[0] - 1;

    if(item == 0)
    {
        puts("No items chosen");
        return 0;
    }
        
    puts("Items chosen:");

    //display the item as (item index, item value) pairs, one per line
    if(values)
    {
        long total_value = 0;
        for(; item > 0; --item)
        {
            total_value += values[items_list[item]];
            printf("item %ld, val %ld\n", items_list[item], values[items_list[item]]);
        }
        //display the total value of the solution
        printf("\ttotal value: %ld\n", total_value);
    }
    //display strictly the item numbers
    else
    {
        for(; item > 0; --item)
        {
            printf("item %ld \n", items_list[item]);
        }
    }

    return 0;
}

/*
    print_solutions_table
        displays the knapsack 0/1 solutions table as a num_items*max_weight matrix
    param:
        int num_items (number of items considered)
        int max_weight (maximum weight considered)  
        int** s_table (the solutions table to display)
    return:
        upon success returns 0 
        if nothing to print, returns -1
*/
int print_solutions_table(long num_items, long max_weight, long** s_table)
{
    if(num_items < 0 || max_weight < 0 || !s_table)return -1;
    
    long itm = 0;
    long wt = 0;
    for(; itm <= num_items; ++itm)
    {
        printf("%c", '{');
        for(wt = 0; wt < max_weight; ++wt)
        {
            printf("%ld, ", s_table[itm][wt]);
        }
        printf("%ld}\n", s_table[itm][wt]);
    }

    return 0;
}
