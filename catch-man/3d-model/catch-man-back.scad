// catch-man back wall
// TODO for r2: 
//  - SLOP can be reduced, PCB is kinda loose
//  - some sort of mounting holes?

WALL = 1;
PCB_HEIGHT = 2; 
SPACE_BEHIND = 3;

SLOP = 0.5;

module outline(delta_r, h) difference() {
    HANDLE_Y = 52.584;
    union() {
        // main body
        cylinder(h=h, r=54.1 + SLOP + delta_r, $fn=100); // r=54.09 nominal    
        // handle
        translate([0, HANDLE_Y, 0])
            cylinder(h=h, r=12.7 + SLOP + delta_r, $fn=100); 
    }
    // hole in handle
    translate([0, HANDLE_Y, 0])
        cylinder(h=h, r=7 - SLOP - delta_r/2, $fn=100); // r=7.0 nominal                    
    
}

difference() {
    translate([0, 0, -WALL])
    outline(delta_r=WALL, h=(PCB_HEIGHT + SPACE_BEHIND + WALL));
    
    // cutout "space behind"
    outline(delta_r=-WALL, h=100);
    
    translate([0, 0, SPACE_BEHIND])
    outline(delta_r=0, h=100);
}