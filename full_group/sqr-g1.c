#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <string.h>

enum iter_type {IterPassive, IterActive, IterPair };

struct StackEl { 
    unsigned char pos;
    unsigned char val2; // pos 2 or rest group size
    unsigned char iter_col_index;
    enum iter_type type; // 0 : passive single, 1 iterating single, 2: iterating pair
    unsigned char group_nr;
};

struct cellVal {
    unsigned char size;
    int el[4][3];
};

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
    
    unsigned char  max_pos          = size * size;                // el of sqr
    unsigned int   group_sum_target = (1 + max_pos) / 2.0 * size; //
    unsigned char  group_count      = 2 * size + 2;
    unsigned char  val_at_pos[ max_pos+1 ];
    unsigned char  pos_of_val[ max_pos+1 ];
    unsigned char  groups_at_pos[ max_pos + 1 ][4];
    unsigned char  pos_of_group[ group_count + 1 ][size];
    unsigned char  group_fill[ group_count + 1 ];
             short group_sum[ group_count + 1 ];
    unsigned short iter_val[ max_pos+1 ][ max_pos+1 ];
    struct StackEl comb_stack[max_pos];
             char  stack_index = 0;
    unsigned char  stack_index_max = 0;
    struct cellVal init_cell_values;

             int i, j, pos, value = 0;                        // fill default vals
    for( i = 0; i <= max_pos; i++ ) val_at_pos[i] = pos_of_val[i] = 0;
    groups_at_pos[0][0] = groups_at_pos[0][1] = groups_at_pos[0][2] = groups_at_pos[0][3] = 0;
    for( i = 1; i <= max_pos; i++ ) {
        unsigned char im = i - 1;
        int row = im / size;
        int col = im % size;
        groups_at_pos[i][0] = 1 + row;
        groups_at_pos[i][1] = 1 + col + size;
        groups_at_pos[i][2] = (row == col) ? (group_count-1) : 0;
        groups_at_pos[i][3] = (row + col+1 == size) ? group_count : 0;
        pos_of_group[ groups_at_pos[i][0] ][ col ] = i;
        pos_of_group[ groups_at_pos[i][1] ][ row ] = i;
        if (groups_at_pos[i][2]) pos_of_group[ group_count-1 ][ row ] = i;
        if (groups_at_pos[i][3]) pos_of_group[ group_count   ][ row ] = i;
    }
    for( i = 0; i <= group_count; i++ ) group_sum[i] = group_fill[i] = 0;

    if (size % 2 == 0) {                                        // set given cells
        struct cellVal icv = {4,  1,1,size,  1,size,1,  size,1,max_pos,  size,size,max_pos-size+1 };
        init_cell_values = icv;
    } else {
        unsigned char mid_col   =  (size + 1) / 2;
        struct cellVal icv = {3, mid_col-1, mid_col, max_pos, 
                                 mid_col,   mid_col, (max_pos+1)/2, 
                                 mid_col+1, mid_col, 1,             0, 0, 0}; 
        init_cell_values = icv;
    }
    for( i = 0; i < init_cell_values.size; i++ ){
        pos = (init_cell_values.el[i][0] - 1) * size + init_cell_values.el[i][1];
        value = init_cell_values.el[i][2];
        val_at_pos[ pos ] = value;
        pos_of_val[ value ] = pos;
        group_sum[ groups_at_pos[pos][0] ] += value;
        group_sum[ groups_at_pos[pos][1] ] += value;
        group_fill[ groups_at_pos[pos][0] ]++;
        group_fill[ groups_at_pos[pos][1] ]++;
        if (groups_at_pos[pos][2]) {
            group_sum[ groups_at_pos[pos][2] ] += value;
            group_fill[ groups_at_pos[pos][2] ]++;
        } 
        if (groups_at_pos[pos][3]) {
            group_sum[ groups_at_pos[pos][3] ] += value;        
            group_fill[ groups_at_pos[pos][3] ]++;
        }
    }
    val_at_pos[ 0 ] = init_cell_values.size;

    unsigned char iter_col_index = 0;
    while (val_at_pos[ 0 ] < max_pos){
        unsigned char max_fill = 0;
        unsigned char group_index = 0;
        for (i = 1; i <= group_count; i++ ) // index of smallest group
            if (max_fill < group_fill[ i ] && group_fill[ i ] != size) {
                max_fill = group_fill[ i ];
                group_index = i;
            }
        unsigned char missing_cells = size - max_fill;
        unsigned char empty_pos[ missing_cells ]; // of current group
        j = 0; // get empty cells of group
        for (i = 0; i < size; i++ )
            if (val_at_pos[ pos_of_group[ group_index ][ i ] ] == 0)
                empty_pos[ j++ ] = pos_of_group[ group_index ][ i ];

        for (i = 0; i < missing_cells; i++ ){ // dummy fill marked cells
            pos = empty_pos[i];
            val_at_pos[ pos ] = ++val_at_pos[ 0 ];
            group_fill[ groups_at_pos[pos][0] ]++;
            group_fill[ groups_at_pos[pos][1] ]++;
            if (groups_at_pos[pos][2])  group_fill[ groups_at_pos[pos][2] ]++;
            if (groups_at_pos[pos][3])  group_fill[ groups_at_pos[pos][3] ]++;
        }

        while (missing_cells > 2) {
            missing_cells--;
            comb_stack[stack_index].type     = IterActive;
            comb_stack[stack_index].pos      = empty_pos[ missing_cells ];
            comb_stack[stack_index].val2     = missing_cells;
            comb_stack[stack_index].group_nr = group_index;
            comb_stack[stack_index].iter_col_index = iter_col_index++;
            stack_index++;
        }
        if (missing_cells == 1)
            comb_stack[stack_index].type     = IterPassive;
        else {
            comb_stack[stack_index].type     = IterPair;
            comb_stack[stack_index].val2     = empty_pos[ 1 ];
            comb_stack[stack_index].iter_col_index = iter_col_index;
            iter_col_index += 2;
        }
        comb_stack[stack_index].pos      = empty_pos[ 0 ];
        comb_stack[stack_index].group_nr = group_index;
        stack_index++;
    }
    
    stack_index_max = --stack_index; // reset stack
    stack_index = 0;
    val_at_pos[ 0 ] = 0;
    for (i = 1; i <= max_pos; i++ )
        if (pos_of_val[ val_at_pos[ i ] ] != i ) val_at_pos[ i ] = 0;
    for (i = 1; i <= group_count; i++ )
        group_sum[i] = group_sum_target - group_sum[i];
        

    printf("\n ");
    for( i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Given: \n");
    print_sqr( size, val_at_pos );
    printf(" ");
    for( i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Solution: \n");
    
    unsigned long int iter_max = 100000000;
    unsigned long int iter_cc = 0;
    unsigned int sol_cc = 0;
    unsigned char smallest_free_val = 1;
    unsigned char largest_free_val = max_pos;
    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
    while (pos_of_val[ largest_free_val ]) largest_free_val--;
    char stack_index_move = 1;
    while (stack_index > -1 && iter_cc < iter_max) {
        struct StackEl stack_cell = comb_stack[stack_index];
        if (stack_cell.type == IterActive){
            if (stack_index_move > 0) {
                iter_val[ stack_cell.iter_col_index ][ 0 ] = 0;
                short missing_sum = group_sum[ stack_cell.group_nr ];
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
                group_sum[ groups_at_pos[pos][0] ] += value;
                group_sum[ groups_at_pos[pos][1] ] += value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] += value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] += value;
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
            }    
            if (iter_val[ stack_cell.iter_col_index ][ 0 ]){ // more to iter through
                pos = stack_cell.pos;
                value =  iter_val[ stack_cell.iter_col_index ][ iter_val[ stack_cell.iter_col_index ][ 0 ]-- ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                group_sum[ groups_at_pos[pos][0] ] -= value;
                group_sum[ groups_at_pos[pos][1] ] -= value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] -= value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] -= value;
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
        } else if (stack_cell.type == IterPair){
            if (stack_index_move > 0) {
                iter_val[ stack_cell.iter_col_index ][ 0 ] = 0;
                short missing_sum = group_sum[ stack_cell.group_nr ];
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
                group_sum[ groups_at_pos[pos][0] ] += value;
                group_sum[ groups_at_pos[pos][1] ] += value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] += value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] += value;
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;

                pos = stack_cell.val2;
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                group_sum[ groups_at_pos[pos][0] ] += value;
                group_sum[ groups_at_pos[pos][1] ] += value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] += value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] += value;
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
            }
            if (iter_val[ stack_cell.iter_col_index ][ 0 ]){ // more to iter through
                pos = stack_cell.pos;
                value =  iter_val[ stack_cell.iter_col_index ][ iter_val[ stack_cell.iter_col_index ][ 0 ] ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                group_sum[ groups_at_pos[pos][0] ] -= value;
                group_sum[ groups_at_pos[pos][1] ] -= value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] -= value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] -= value;
                if (smallest_free_val == value)
                    while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                if (largest_free_val == value)
                    while (pos_of_val[ largest_free_val ]) largest_free_val--;

                pos = stack_cell.val2;
                value =  iter_val[ stack_cell.iter_col_index + 1 ][ iter_val[ stack_cell.iter_col_index ][ 0 ]-- ];
                pos_of_val[ value ] = pos;
                val_at_pos[ pos ] = value;
                group_sum[ groups_at_pos[pos][0] ] -= value;
                group_sum[ groups_at_pos[pos][1] ] -= value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] -= value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] -= value;
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
        } else if (stack_cell.type == IterPassive){
            pos = stack_cell.pos;
            if (stack_index_move < 0) { // upward slide , remove set value
                value = val_at_pos[ pos ];
                pos_of_val[ value ] = val_at_pos[ pos ] = 0;
                group_sum[ groups_at_pos[pos][0] ] += value;
                group_sum[ groups_at_pos[pos][1] ] += value;
                if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] += value;
                if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] += value;
                if (smallest_free_val > value) smallest_free_val = value;
                if (largest_free_val < value) largest_free_val = value;
                stack_index--;
            } else {
                value = group_sum[ stack_cell.group_nr ];

                if (value < smallest_free_val || value > largest_free_val || pos_of_val[ value ]){ // or only possible value taken
                    stack_index--;
                    stack_index_move = -1;
                } else {
                    val_at_pos[ pos ] = value;
                   
                    if (stack_index == stack_index_max){
                        print_sqr( size, val_at_pos );
                        sol_cc++;
                        val_at_pos[ pos ] = 0;
                        stack_index_move = -1;
                        stack_index--;
                    } else {
                        pos_of_val[ value ] = pos;
                        group_sum[ groups_at_pos[pos][0] ] -= value;
                        group_sum[ groups_at_pos[pos][1] ] -= value;
                        if (groups_at_pos[pos][2]) group_sum[ groups_at_pos[pos][2] ] -= value;
                        if (groups_at_pos[pos][3]) group_sum[ groups_at_pos[pos][3] ] -= value;
                        if (smallest_free_val == value)
                            while (pos_of_val[ smallest_free_val ]) smallest_free_val++;
                        if (largest_free_val == value)
                            while (pos_of_val[ largest_free_val ]) largest_free_val--;
                        stack_index++;
                    }
                }
            }
        }
        iter_cc++;
    }
    double time_taken = ((double) (clock() - t) )/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("done %lu iterations on square size %lu, found %u solutions in %f sec \n", iter_cc, size, sol_cc, time_taken );
    return 0;
}

/*
 * count everything to zero
 * struct for diff sized group
 * 
 * */
