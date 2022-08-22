#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <string.h>

void print_sqr (unsigned char size, unsigned char val_at_pos[]){
    unsigned char max_pos = size * size;                // el of sqr
    printf(" ");
    for(int i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n\n");
    for(int i = 1; i <= max_pos; i++ ){
        printf("  %2u", val_at_pos[i] );
        if (i % size == 0 && i != max_pos)  printf("\n\n");
    }
    printf("\n\n");
}

int main(int argc, char **argv) {
    clock_t t = clock();
    char *ptr;
    unsigned long int size = 0;
    if (argc != 2) return printf("need one numeric argument: size of square ( 3 - 12 ) !\n");
    else           size =  strtoul(argv[1], &ptr, 10);
    if  (size < 3 || size > 12) return printf("size of square has to be: 3 - 12!\n");


    struct Group {
        unsigned char cell_count;
        unsigned char nr;
                 char missing_count;
                  int missing_sum;
        unsigned char cells[size];
    };
    unsigned char  max_pos           = size * size;           // el of sqr
    float          avg_cell_value    = (1 + max_pos) / 2.0;
    unsigned char  half_size         = size / 2;
    unsigned char  size_odd          = size % 2;
    unsigned char  full_groups       = size * 2;
    unsigned char  even_pair_groups  = half_size * half_size * 2;
    unsigned char  odd_pair_groups   = half_size * size_odd * 2;
    unsigned char  group_count       = full_groups + even_pair_groups + odd_pair_groups;
    struct Group   groups[ group_count + 1 ];
    unsigned char  groups_at_pos[ max_pos + 1 ][3];

    int i = 0, j = 0, pos = 0, value = 0;
    for( i = 0;             i <= group_count; i++ )   groups[i].nr            = i;
    for( i = 1;             i <= full_groups; i++ )   groups[i].missing_count = groups[i].cell_count = size;
    for( i = full_groups+1; i <= group_count; i++ )   groups[i].missing_count = groups[i].cell_count = 2;
    for( i = 1;             i <= group_count; i++ )   groups[i].missing_sum   = groups[i].cell_count * avg_cell_value;
    for( i = 1; i <= max_pos; i++ ) {                  // put cells into groups
        int row = (i - 1) / size;
        int col = (i - 1) % size;
        groups_at_pos[i][0] = 1 + row;        //       1 ..   size - horizontal
        groups_at_pos[i][1] = 1 + col + size; // size + 1 .. 2 size - vertical
        groups[1 + row].cells[col] = i;
        groups[1 + col + size].cells[row] = i;
    }

    int group_nr = full_groups + 1;
    for( j = 1; j <= half_size; j++ ) {
        for( i = 1; i <= half_size; i++ ) {
            pos = (j - 1) * size + i;
            unsigned char pos2 = (j - 1) * size + (size + 1 - i);
            unsigned char pos3 = (size - j) * size + i;
            unsigned char pos4 = (size - j) * size + (size + 1 - i);

            if (i !=j && size == 6) {
                int vertical_pairs = (i > j) ? i - j : 1 + j - i;
                if (vertical_pairs % 2) {
                    groups_at_pos[pos ][2] = group_nr; // pair groups
                    groups_at_pos[pos2][2] = group_nr + 1;
                    groups_at_pos[pos3][2] = group_nr;
                    groups_at_pos[pos4][2] = group_nr + 1;
                    groups[ group_nr     ].cells[ 0 ] = pos;
                    groups[ group_nr + 1 ].cells[ 0 ] = pos2;
                    groups[ group_nr     ].cells[ 1 ] = pos3;
                    groups[ group_nr + 1 ].cells[ 1 ] = pos4;
                } else {
                    groups_at_pos[pos ][2] = group_nr; // pair groups
                    groups_at_pos[pos2][2] = group_nr;
                    groups_at_pos[pos3][2] = group_nr + 1;
                    groups_at_pos[pos4][2] = group_nr + 1;
                    groups[ group_nr     ].cells[ 0 ] = pos;
                    groups[ group_nr     ].cells[ 1 ] = pos2;
                    groups[ group_nr + 1 ].cells[ 0 ] = pos3;
                    groups[ group_nr + 1 ].cells[ 1 ] = pos4;
                }
            } else {
                groups_at_pos[pos ][2] = group_nr; // pair groups
                groups_at_pos[pos2][2] = group_nr + 1;
                groups_at_pos[pos3][2] = group_nr + 1;
                groups_at_pos[pos4][2] = group_nr;
                groups[ group_nr     ].cells[ 0 ] = pos;
                groups[ group_nr + 1 ].cells[ 0 ] = pos2;
                groups[ group_nr + 1 ].cells[ 1 ] = pos3;
                groups[ group_nr     ].cells[ 1 ] = pos4;
            }
            group_nr += 2;
        }
    }
    if (size_odd){ // rhomb groups  + center cell
        int mid_size = (size + 1) / 2;
        pos = (mid_size - 1) * size + mid_size;
        groups_at_pos[pos][2] = 0; // diag
        int group_nr = full_groups + even_pair_groups + 1;
        
        for( i = 1; i <= half_size; i++ ) {
                pos  = ((i - 1)        * size) + mid_size;
            int pos2 = (mid_size - 1) * size + i;
            int pos3 =  mid_size      * size - i + 1;
            int pos4 = (size - i)     * size + mid_size;
            groups_at_pos[pos ][2] = group_nr; // pair groups
            groups_at_pos[pos2][2] = group_nr + 1;
            groups_at_pos[pos3][2] = group_nr + 1;
            groups_at_pos[pos4][2] = group_nr;
            groups[ group_nr     ].cells[ 0 ] = pos;
            groups[ group_nr     ].cells[ 1 ] = pos4;
            groups[ group_nr + 1 ].cells[ 0 ] = pos2;
            groups[ group_nr + 1 ].cells[ 1 ] = pos3;
            group_nr += 2;
        }    
    }

    struct cellVal {                                       // set given cells ////////
        unsigned char size;
        int el[4][3];
    };
    struct cellVal init_cell_values;
 
    if (size % 2) {
        unsigned char mid_col   =  (size + 1) / 2;
        struct cellVal icv = {3, mid_col-1, mid_col, max_pos, 
                                 mid_col,   mid_col, avg_cell_value, 
                                 mid_col+1, mid_col, 1,             0, 0, 0}; 
        init_cell_values = icv;
    } else {
        struct cellVal icv = {4,  1,1,size,  1,size,1,  size,1,max_pos,  size,size,max_pos-size+1 };
        init_cell_values = icv;
    }

    unsigned char  val_at_pos[ max_pos+1 ];
    unsigned char  pos_of_val[ max_pos+1 ];
    for( i = 0; i <= max_pos; i++ )  val_at_pos[i] = pos_of_val[i] = 0;
    for( i = 0; i < init_cell_values.size; i++ ){
        pos = ((init_cell_values.el[i][0] - 1) * size) + init_cell_values.el[i][1];
        value = init_cell_values.el[i][2];

        val_at_pos[ pos ] = value;
        pos_of_val[ value ] = pos;
        
        groups[ groups_at_pos[pos][0] ].missing_sum -= value;
        groups[ groups_at_pos[pos][0] ].missing_count--;
        groups[ groups_at_pos[pos][1] ].missing_sum -= value;
        groups[ groups_at_pos[pos][1] ].missing_count--;
        groups[ groups_at_pos[pos][2] ].missing_sum -= value;
        groups[ groups_at_pos[pos][2] ].missing_count--;
    }
    val_at_pos[ 0 ] = init_cell_values.size;
    
    enum iter_type {IterCheck, IterFill, IterSingle, IterPair }; // figure out solfing path ////////

    struct StackEl { 
        unsigned char pos;
        unsigned char val2; // pos 2 or rest group size
        unsigned char iter_col_index;
        enum iter_type type; // 0 : passive single, 1 iterating single, 2: iterating pair
        unsigned char group_nr;
    };
    struct StackEl comb_stack[max_pos];
             char  stack_index = 0;
    unsigned short iter_val[ max_pos+1 ][ max_pos+1 ];
    unsigned char iter_col_index = 0;


   
    while (val_at_pos[ 0 ] < max_pos) { 
        unsigned char missing_cells = size+1;
        unsigned char group_index = 0;
        unsigned char stack_inc = 1;
        for (i = 1; i <= group_count; i++ ) // index of group with least amount of missing cells
            if (groups[i].missing_count && missing_cells > groups[i].missing_count) {
                missing_cells = groups[i].missing_count;
                group_index = i;
            }
   //printf(" - group %u misses %u\n", group_index, missing_cells);

        unsigned char empty_pos_in_group[ missing_cells ]; // of current group
        j = 0;
        for (i = 0; i < groups[group_index].cell_count; i++ ){  // get empty cells of group
            if (val_at_pos[ groups[group_index].cells[ i ] ]) continue;
            empty_pos_in_group[ j++ ] = groups[group_index].cells[ i ];
        }
        for (i = 0; i < missing_cells; i++ ){ // dummy fill marked cells
            pos = empty_pos_in_group[i];
            val_at_pos[ pos ] = ++val_at_pos[ 0 ];
            for (j = 0; j <= 2; j++ ){
                groups[ groups_at_pos[pos][j] ].missing_count--;
                if (groups_at_pos[pos][j] != group_index && groups[ groups_at_pos[pos][j] ].missing_count == 0){
                    comb_stack[stack_index + stack_inc].type = IterCheck; 
                    comb_stack[stack_index + stack_inc].group_nr = groups_at_pos[pos][j]; 
                    stack_inc++;
                }
            }
        }

        while (missing_cells > 2) {
            missing_cells--;
            comb_stack[stack_index].type     = IterSingle;
            comb_stack[stack_index].pos      = empty_pos_in_group[ missing_cells ];
            comb_stack[stack_index].val2     = missing_cells;
            comb_stack[stack_index].group_nr = group_index;
            comb_stack[stack_index].iter_col_index = iter_col_index++;
            stack_index++;
        }
        if (missing_cells == 1)
            comb_stack[stack_index].type     = IterFill;
        else {
            comb_stack[stack_index].type     = IterPair;
            comb_stack[stack_index].val2     = empty_pos_in_group[ 1 ];
            comb_stack[stack_index].iter_col_index = iter_col_index;
            iter_col_index += 2;
        }
        comb_stack[stack_index].pos      = empty_pos_in_group[ 0 ];
        comb_stack[stack_index].group_nr = group_index;
        stack_index += stack_inc; // print_sqr( size, val_at_pos );
// for(i = 1; i <= group_count; i++)  printf(" %u group ( %u %u ) miss :  %i\n", i, groups[i].cells[0], groups[i].cells[1], groups[i].missing_count);
    }

// for( i = 0; i < stack_index; i++ )     printf("stack el %i has type %i  of group %u \n", i, comb_stack[i].type, comb_stack[i].group_nr);


    unsigned char stack_index_max = stack_index - 1; // reset stack and values in square
    stack_index = 0;
    val_at_pos[ 0 ] = 0;
    for (i = 1; i <= max_pos; i++ )
        if (pos_of_val[ val_at_pos[ i ] ] != i ) val_at_pos[ i ] = 0;
       
    printf("\n ");
    for( i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Given: \n");
    print_sqr( size, val_at_pos );
    printf(" ");
    for( i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Solution: \n");
    
    unsigned long int iter_max = 10000000000; // 100000000
    unsigned long int iter_cc = 0;
    unsigned int sol_cc = 0;
    unsigned int bad_sol = 0;
    unsigned char smallest_free_val = 1;
    unsigned char largest_free_val = max_pos;
    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
    while (pos_of_val[ largest_free_val ]) largest_free_val--;
    char stack_index_move = 1;

    while (stack_index > -1 && iter_cc < iter_max) {
        struct StackEl stack_cell = comb_stack[stack_index];
        if (stack_cell.type == IterPair){ // printf(" Iter Pair , stack %i, group %u \n", stack_index, stack_cell.group_nr );
            if (stack_index_move > 0) {
                iter_val[ stack_cell.iter_col_index ][ 0 ] = 0;
                short missing_sum = groups[ stack_cell.group_nr ].missing_sum;
                for (int value2 = smallest_free_val; value2 <= largest_free_val; value2++ ){
                    if (value2 + smallest_free_val > missing_sum) break;
                    if (pos_of_val[ value2 ]) continue;
                    value = missing_sum - value2;
                    if (value < smallest_free_val) break;
                    if (value > largest_free_val) continue;
                    if (pos_of_val[ value ]) continue;
                    if (value == value2) continue;
                    
                    iter_val[ stack_cell.iter_col_index ]    [ 0 ]++;
                    iter_val[ stack_cell.iter_col_index ]    [ iter_val[ stack_cell.iter_col_index ][ 0 ] ] = value;
                    iter_val[ stack_cell.iter_col_index + 1 ][ iter_val[ stack_cell.iter_col_index ][ 0 ] ] = value2;
                }
                stack_index_move = 0;

            } else if (stack_index_move < 0) {
                pos = stack_cell.pos;
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                groups[ groups_at_pos[pos][0] ].missing_sum += value;
                groups[ groups_at_pos[pos][1] ].missing_sum += value;
                groups[ groups_at_pos[pos][2] ].missing_sum += value;

                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;

                pos = stack_cell.val2;
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                groups[ groups_at_pos[pos][0] ].missing_sum += value;
                groups[ groups_at_pos[pos][1] ].missing_sum += value;
                groups[ groups_at_pos[pos][2] ].missing_sum += value;
                
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
            }
            if (iter_val[ stack_cell.iter_col_index ][ 0 ]){ // more to iter through
                pos = stack_cell.pos;
                value =  iter_val[ stack_cell.iter_col_index ][ iter_val[ stack_cell.iter_col_index ][ 0 ] ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                groups[ groups_at_pos[pos][0] ].missing_sum -= value;
                groups[ groups_at_pos[pos][1] ].missing_sum -= value;
                groups[ groups_at_pos[pos][2] ].missing_sum -= value;

                if (smallest_free_val == value)
                    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                if (largest_free_val == value)
                    while (pos_of_val[ largest_free_val ]) largest_free_val--;

                pos = stack_cell.val2;
                value =  iter_val[ stack_cell.iter_col_index + 1 ][ iter_val[ stack_cell.iter_col_index ][ 0 ]-- ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                groups[ groups_at_pos[pos][0] ].missing_sum -= value;
                groups[ groups_at_pos[pos][1] ].missing_sum -= value;
                groups[ groups_at_pos[pos][2] ].missing_sum -= value;

                if (smallest_free_val == value)
                    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                if (largest_free_val == value)
                    while (pos_of_val[ largest_free_val ]) largest_free_val--;

                stack_index++;
                stack_index_move = 1;
            } else { // iter val taken
                stack_index--;
                stack_index_move = -1;
            }
        } else if (stack_cell.type == IterSingle){ // printf(" Iter Single %i \n", stack_index );


            if (stack_index_move > 0) {
                iter_val[ stack_cell.iter_col_index ][ 0 ] = 0;
                short missing_sum = groups[ stack_cell.group_nr ].missing_sum;
                char rest_elem_count = stack_cell.val2;
                unsigned short min_rest_sum = smallest_free_val;
                unsigned short max_rest_sum = largest_free_val;

                j = 1;
                for (i = smallest_free_val+1; i <= largest_free_val; i++ ){
                    if (pos_of_val[ i ]) continue;
                    min_rest_sum += i;
                    if (++j == rest_elem_count) break;
                }
                j = 1;
                for (i = largest_free_val-1; i > smallest_free_val; i-- ){
                    if (pos_of_val[ i ]) continue;
                    max_rest_sum += i;
                    if (++j == rest_elem_count) break;
                }
                int min_val = missing_sum - max_rest_sum;
                int max_val = missing_sum - min_rest_sum;
                if (min_val < smallest_free_val) min_val = smallest_free_val;
                if (max_val > largest_free_val)  max_val = largest_free_val;
                for (value = min_val; value <= max_val; value++ ){
                    if (pos_of_val[ value ]) continue;
                    iter_val[ stack_cell.iter_col_index ] [ ++iter_val[ stack_cell.iter_col_index ][ 0 ] ] = value;
                }
                stack_index_move = 0;
            } else if (stack_index_move < 0) {
                pos = stack_cell.pos;
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                groups[ groups_at_pos[pos][0] ].missing_sum += value;
                groups[ groups_at_pos[pos][1] ].missing_sum += value;
                groups[ groups_at_pos[pos][2] ].missing_sum += value;

                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
            }    
            if (iter_val[ stack_cell.iter_col_index ][ 0 ]){ // more to iter through
                pos = stack_cell.pos;
                value =  iter_val[ stack_cell.iter_col_index ][ iter_val[ stack_cell.iter_col_index ][ 0 ]-- ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                groups[ groups_at_pos[pos][0] ].missing_sum -= value;
                groups[ groups_at_pos[pos][1] ].missing_sum -= value;
                groups[ groups_at_pos[pos][2] ].missing_sum -= value;

                if (smallest_free_val == value)
                    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                if (largest_free_val == value)
                    while (pos_of_val[ largest_free_val ]) largest_free_val--;
                stack_index++;
                stack_index_move = 1;
            } else { // iter val taken
                stack_index--;
                stack_index_move = -1;
            }
        } else if (stack_cell.type == IterCheck){ // printf(" Iter Check,  stack %i,  group %u \n", stack_index, stack_cell.group_nr );
            if (stack_index_move < 0) stack_index--;
            else if (groups[ stack_cell.group_nr ].missing_sum){
                    stack_index_move = -1;
                    stack_index--;
                } else {
                    if (stack_index == stack_index_max){
                        sol_cc++;
                        // print_sqr( size, val_at_pos );

                        stack_index_move = -1;
                        stack_index--;
                    } else stack_index++;
                }
        } else if (stack_cell.type == IterFill){
            pos = stack_cell.pos; // printf(" Iter Fill, stack: %i, pos: %u \n", stack_index, pos );
            if (stack_index_move < 0) { // upward slide , remove set value
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                groups[ groups_at_pos[pos][0] ].missing_sum += value;
                groups[ groups_at_pos[pos][1] ].missing_sum += value;
                groups[ groups_at_pos[pos][2] ].missing_sum += value;
                
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
                stack_index--;
            } else {
                value = groups[ stack_cell.group_nr ].missing_sum;

                if (value < smallest_free_val || value > largest_free_val || pos_of_val[ value ]){ // or only possible value taken
                    stack_index--;
                    stack_index_move = -1;
                } else {
                    val_at_pos[ pos ] = value;
                    pos_of_val[ value ] = pos;
                    groups[ groups_at_pos[pos][0] ].missing_sum -= value;
                    groups[ groups_at_pos[pos][1] ].missing_sum -= value;
                    groups[ groups_at_pos[pos][2] ].missing_sum -= value;
                   
                    if (stack_index == stack_index_max){
                        //int error_sum = 0;
                        // for (i = 1; i <= group_count; i++ ) error_sum += abs(groups[i].missing_sum);
                        // print_sqr( size, val_at_pos );
                        sol_cc++;
                        
                        // if (error_sum){
                            //for(i = 1; i <= group_count; i++) 
                            //    printf(" %u group ( %u %u ) sum :  %i\n", i, groups[i].cells[0], groups[i].cells[1], groups[i].missing_sum);
                            //print_sqr( size, val_at_pos );
                            // bad_sol++;
                        // } else { }
                        pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                        groups[ groups_at_pos[pos][0] ].missing_sum += value;
                        groups[ groups_at_pos[pos][1] ].missing_sum += value;
                        groups[ groups_at_pos[pos][2] ].missing_sum += value;
                        stack_index_move = -1;
                        stack_index--;
                    } else {
                        if (smallest_free_val == value)
                            while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                        if (largest_free_val == value)
                            while (pos_of_val[ largest_free_val ]) largest_free_val--;
                        stack_index++;
                    }
                }
            }
        }
        iter_cc++; //  print_sqr( size, val_at_pos );  for( i = 1; i <= group_count; i++ )     printf(" iter %lu  %u group ( %u %u ) sum :  %i\n", iter_cc, i, groups[i].cells[0], groups[i].cells[1], groups[i].missing_sum);
    }
    double time_taken = ((double) (clock() - t) )/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("done %lu iterations on square size %lu, found %u solutions in %f sec \n", iter_cc, size, sol_cc, time_taken );
    return 0;
}
