use v5.12;
use Benchmark;

###################### ARGS ############################################
my $timer = Benchmark->new;  
my $size = int shift;
exit say "need a number between 3 and 12 as first argument (square size)" 
    unless defined $size and $size > 2 and  $size < 13;
my $print_sol = shift;

my $half_size         = int $size / 2;
my $size_odd          = $size % 2;
my $max_value = 
my $max_pos           = $size ** 2;
my $pair_value        = $max_value + 1;
my $mid_pos           = int $max_pos / 2;
my $mid_row           = ($size+1) / 2;
my $avg_cell_value    = $pair_value / 2;
my $full_group_value  = $size * $avg_cell_value;
my $full_groups       = $size * 2;
my $pair_groups       = int $max_pos / 2;
my $group_count       = $full_groups + $pair_groups;

#### construct groups ##################################################
my (@group, @groups_at_pos, @group_sum);

$group[$_]->{'nr'} = $_ for 0 .. $group_count;
$group[0]->{'size'} = $group[0]->{'miss'} = 0;
$group[$_]->{'size'} = $group[$_]->{'miss'} = $size for 1 .. $full_groups;
$group[$_]->{'size'} = $group[$_]->{'miss'} = 2 for $full_groups + 1 .. $group_count;
for my $pos (1 .. $max_pos) {
    my $row = int (($pos - 1) / $size);
    my $col = ($pos - 1) % $size;
    $group[1 + $row]->{'cell'}[$col] = $pos;
    $group[1 + $col + $size]->{'cell'}[$row] = $pos;
    next if $pos > $mid_pos;
    my $pos2 =  (( $size - $row ) * $size) - $col;
    my $group_nr = $full_groups + $pos;
    $group[ $group_nr ]->{'cell'}[0] = $pos;
    $group[ $group_nr ]->{'cell'}[1] = $pos2;
}
for my $group_nr (1 .. $group_count) {
    push @{$groups_at_pos[$_]}, $group_nr for @{$group[ $group_nr ]->{'cell'}};
    $group_sum[ $group_nr ] = $group[ $group_nr ]->{'size'} * $avg_cell_value;
}

#### set init values ###################################################
my @init = ($size % 2)
         ? ([$mid_row-1, $mid_row, $max_value], [$mid_row, $mid_row, $avg_cell_value], [$mid_row+1, $mid_row, 1], )
         : ([1,1,$size], [1,$size,1], [$size,1,$max_value], [$size,$size,$max_value-$size-1],);

my (@val_at_pos, @pos_of_val);
$val_at_pos[$_] = $pos_of_val[$_] = 0 for 0 .. $max_pos;

for my $cell_def (@init){
    my $pos = ($cell_def->[0] - 1) * $size + $cell_def->[1];
    my $value = $cell_def->[2];
    $val_at_pos[ $pos ] = $value;
    $pos_of_val[ $value ] = $pos;
    $group_sum[ $groups_at_pos[$pos][$_] ] -= $value for 0 .. 2;
    $group[ $groups_at_pos[$pos][$_] ]{'miss'}-- for 0 .. 2;
}

#### construct iter path ###############################################
my ($iterStack, $stack_pos, $pos, $value, $back) = ([], 0, 0, 0, -1);
my $open_cells = $max_pos - @init;

while ($open_cells){
    my $missing_in_group = $size;
    my $group_nr = 0;
    for (1 .. $group_count) {
        next unless $group[$_]->{'miss'} and $group[$_]->{'miss'} < $missing_in_group;
        $missing_in_group = $group[$_]->{'miss'};
        $group_nr = $_;
    }
    my @missing_pos = grep { not $val_at_pos[ $_ ]} @{$group[ $group_nr ]->{'cell'}};

    my $stack_inc = 0;
    while (@missing_pos){
        if (@missing_pos == 1) {       # fill - checks only here and in dual
            $pos = pop @missing_pos;
            $val_at_pos[ $pos ] = -($max_pos - --$open_cells);
            $group[$_]{'miss'}-- for @{$groups_at_pos[$pos]};
            my @completing_groups = grep {not $group[$_]{'miss'}} @{$groups_at_pos[$pos]};
            $iterStack->[$stack_pos++] = { type => 0, group => \@completing_groups, pos => $pos, prev_iter => $back};
            $back--;
        } elsif (@missing_pos == 2) {  # dual iter
            $pos = pop @missing_pos;
            $val_at_pos[ $pos ] = -($max_pos - --$open_cells);
            $group[$_]{'miss'}-- for @{$groups_at_pos[$pos]};
            $pos = pop @missing_pos;
            $val_at_pos[ $pos ] = -($max_pos - $open_cells--);
            $group[$_]{'miss'}-- for @{$groups_at_pos[$pos]};
            my @completing_groups = grep {not $group[$_]{'miss'}} @{$groups_at_pos[$pos]};
            $iterStack->[$stack_pos++] = 
                { type => 2, group => (@completing_groups >1) ? \@completing_groups : $group_nr, 
                             pos => $missing_pos[0], pos2 => $missing_pos[1], prev_iter => $back, iter_var => 0 };
            $back = -1;
        } else {                       # single iter
            $pos = pop @missing_pos;
            $val_at_pos[ $pos ] = -($max_pos - --$open_cells);
            $group[$_]{'miss'}-- for @{$groups_at_pos[$pos]};
            $iterStack->[$stack_pos++] = { type => 1, group => $group_nr, pos => $pos, prev_iter => $back, iter_var => 0 };
            $back = -1;
        }
    }
}
$val_at_pos[ $_ ] = $val_at_pos[ $_ ] < 0 ? 0 : $val_at_pos[ $_ ] for 1 .. $max_pos; # reset vals
print_sqr( \@val_at_pos );

#### iter ##############################################################
my $iter_max = 10;
my $iter_cc = my $sol_cc = 0;
$stack_pos = 1;
my @open_values = grep {not $val_at_pos[$_]} 1.. $max_value;
unshift @$iterStack, {pos => \@pos_of_val, sum => \@group_sum, open => @open_values};
while ($stack_pos){
    my $stack_el = $iterStack->[$stack_pos];
    if ($stack_el->{'type'} == 0){             # fill 
    } elsif ($stack_el->{'type'} == 1){        # single iter
        if ($stack_el->{'iter_var'}){
        } else {
        }
    } elsif ($stack_el->{'type'} == 2){        # dual iter
        if ($stack_el->{'iter_var'}){
        } else {
        }
    }
    if ($stack_pos == @$iterStack){
        $sol_cc++;
        if ($print_sol){
            $val_at_pos[ pos_of_val[ $_ ] ] = $_ for 1 .. $max_value;
            print_sqr( \@val_at_pos );
        }
    }
    last if $iter_cc++ > $iter_max;
}

# my (@val_at_pos, $group_nr, $pos,  $val $type @open_pos, @open_value
# my ($min_open_pos, $max_open_pos, $min_open_value, $max_open_value);

say " $iter_cc iter found $sol_cc sol in a $size sqare - it took:",timestr(timediff( Benchmark->new, $timer));


sub print_sqr {
    my $val_at_pos = shift;
    say (" ", '-' x ($size * 4), "\n");
    for (1 .. $max_value){
        printf("  %2i", $val_at_pos->[$_] );
        say("\n") if $_ % $size == 0 and $_ != $max_value;
    }
    say("\n");
}
