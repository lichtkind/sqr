#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int str2u ( char str[] ) {
    unsigned int u = 0;
    unsigned char i = 0;
    while (str[i]){
        u *= 10;
        u += (str[i++] - '0');
    }
    return u;
}

void print_sqr (unsigned char size, unsigned char max_pos, unsigned char val_at_pos[]){
    printf(" ");
    for(int i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n\n");
    for(int i = 1; i <= max_pos; i++ ){
        printf("  %2u", val_at_pos[i] );
        if (i % size == 0 && i != max_pos)  printf("\n\n");
    }
    printf("\n\n");
}

void set_cell (unsigned char row,  unsigned char col,  unsigned char value,
               unsigned char size, unsigned char max_pos,
               unsigned char val_at_pos[], unsigned char pos_of_val[],
               unsigned short group_sum[], unsigned short group_fill[],
               unsigned char groups_at_pos[][4], unsigned char solving_path[][2] ){
    unsigned char pos = (row - 1) * size + col;
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
    solving_path[0][0]++;   // total nr
    solving_path[0][1]++;   // nr of set
    solving_path[ solving_path[0][0] ][0] = pos;
    solving_path[ solving_path[0][0] ][1] = value;
}

int main(int argc, char **argv) {
    clock_t t = clock();
    unsigned char size = 0;
    if (argc != 2) return printf("need one numeric argument: size of square ( 3 - 12 ) !\n");
     else {
        size = str2u( argv[1] );
        if  (size < 3 || size > 12) return printf("size of square has to be: 3 - 12!\n");

    }
    unsigned char max_pos          = size * size;                // el of sqr
    unsigned int  group_sum_target = (1 + max_pos) / 2.0 * size; //
    unsigned char group_count      = 2 * size + 2 ;
    unsigned char val_at_pos[ max_pos+1 ];
    unsigned char pos_of_val[ max_pos+1 ];
    unsigned char solving_path[ max_pos + 1 ][2]; // 0:[currently filled, set]
    unsigned char groups_at_pos[ max_pos + 1 ][4];
    unsigned short group_sum[ group_count + 1 ];
    unsigned short group_fill[ group_count + 1 ];

    unsigned int i, j = 0;                        // fill default vals
    for( i = 0; i <= max_pos; i++ ) val_at_pos[i] = pos_of_val[i] = solving_path[i][0] = solving_path[i][1] = 0;
    groups_at_pos[0][0] = groups_at_pos[0][1] = 0;
    for( i = 1; i <= max_pos; i++ ) {
        unsigned char im = i - 1;
        int row = im / size;
        int col = im % size;
        groups_at_pos[i][0] = 1 + row;
        groups_at_pos[i][1] = 1 + col + size;
        groups_at_pos[i][2] = (row == col) ? (group_count-1) : 0;
        groups_at_pos[i][3] = (row + col+1 == size) ? group_count : 0;
    }
    for( i = 0; i <= group_count; i++ ) group_sum[i] = group_fill[i] = 0;

    if (size % 2 == 0) {                          // set given cells
        set_cell(   1,    1,    size, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
        set_cell(   1, size,       1, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
        set_cell(size,    1, max_pos, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
        set_cell(size, size, max_pos-size+1, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);

    } else {
        unsigned char mid_col = (size + 1) / 2;
        set_cell( mid_col-1, mid_col, max_pos, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
        set_cell( mid_col  , mid_col, (max_pos+1)/2, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
        set_cell( mid_col+1, mid_col,       1, size, max_pos, val_at_pos, pos_of_val, group_sum, group_fill, groups_at_pos, solving_path);
    }

    printf("\n ");
    for(int i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Given: \n");
    print_sqr( size, max_pos, val_at_pos );
    printf(" ");
    for(int i = 1; i <= (size * 4); i++ ) printf("-");
    printf("\n  Solution: \n");

    unsigned long int iter_max = 500000000;
    unsigned long int iter_cc = 0;
    unsigned int sol_cc = 0;
    unsigned char pos = 0;
    unsigned char value = 1;
    unsigned char missing_values = max_pos - solving_path[0][1];
    unsigned int limit_sum[ size ][ missing_values ];
    i = 0;
    while (value <= max_pos){
        while (pos_of_val[value]) value++;
        limit_sum[0][i++] = value++;
    }
    for(int i = 1; i < size; i++ )
        for(int j = 0; j < missing_values - i; j++ )
            limit_sum[i][j] = limit_sum[i-1][j] + limit_sum[0][i+j];
    value = limit_sum[0][missing_values - 1];

    while (1) {
LOOP:   pos++;
        while (pos <= max_pos && val_at_pos[pos]) pos++;
        if (pos > max_pos){
            if (solving_path[0][0] == solving_path[0][1]) break; // iterated over all, dont cut into task (set elems)
            pos = solving_path[ solving_path[0][0] ][0];         // take solution element from stack
            value = solving_path[ solving_path[0][0] ][1];
            solving_path[0][0]--;
            missing_values++;
            val_at_pos[ pos ] = pos_of_val[ value ] = 0;
            group_sum[ groups_at_pos[pos][0] ] -= value;         // group settings back (without teration)
            group_sum[ groups_at_pos[pos][1] ] -= value;
            group_fill[ groups_at_pos[pos][0] ]--;
            group_fill[ groups_at_pos[pos][1] ]--;
            if (groups_at_pos[pos][2]) {
                group_sum[ groups_at_pos[pos][2] ] -= value;
                group_fill[ groups_at_pos[pos][2] ]--;
            } 
            if (groups_at_pos[pos][3]) {
                group_sum[ groups_at_pos[pos][3] ] -= value;        
                group_fill[ groups_at_pos[pos][3] ]--;
            }
            continue;
        }

        for( i = 0; i < 4; i++ ){
            unsigned char group_nr = groups_at_pos[pos][i];
            if (group_nr == 0) continue;
            if (group_fill[ group_nr ] == 0) continue;
            int missing_sum = group_sum_target - group_sum[ group_nr ] - value;
            unsigned char empty_cells = size - group_fill[ group_nr ] - 1;
            if (empty_cells){
                if (missing_sum < 0) goto LOOP;
                if (limit_sum[ empty_cells-1 ][0] > missing_sum) goto LOOP;  // undercut lower limit
                if (limit_sum[ empty_cells ][missing_values - empty_cells - 1] < missing_sum + value) goto LOOP; // over the upper
                if ((empty_cells == 1) && (pos_of_val[missing_sum])) goto LOOP;
            } else if (missing_sum != 0) goto LOOP;
        }
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
        solving_path[0][0]++;
        solving_path[ solving_path[0][0] ][0] = pos;
        solving_path[ solving_path[0][0] ][1] = value;
        if (iter_cc++ > iter_max) break;                // iteration max run out
        
        value = limit_sum[0][ --missing_values - 1];
        if (missing_values == 0){
            print_sqr( size, max_pos, val_at_pos );
            sol_cc++;
            pos = max_pos;
        } else pos = 0;
    }

    double time_taken = ((double) (clock() - t) )/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("done %lu iterations on square size %u, found %u solutions in %f sec \n", iter_cc, size, sol_cc, time_taken);
    return 0;
}
