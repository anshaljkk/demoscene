// 3d renderer attempt
// ansh - started this at like 1am so some of it makes no sense
// TODO: clean this up

#include <stdio.h>
#include <math.h>
#include <unistd.h>

// idk what size to make these
#define W 80
#define H 40

// tried making a vec3 struct but just using separate floats for now
float camx = 0, camy = 0, camz = -3;

// sdf for a sphere - copied the formula from wikipedia and it works somehow
float sphere(float px, float py, float pz, float r) {
    return sqrt(px*px + py*py + pz*pz) - r;
}

// i think this is how you normalize a vector
void normalize(float* x, float* y, float* z) {
    float len = sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));
    // BUG: crashes if len is 0, fix later
    *x = *x / len;
    *y = *y / len;
    *z = *z / len;
}

// scene function - only one sphere for now
float scene(float x, float y, float z) {
    return sphere(x, y, z, 1.0);
    // wanted to add a torus but couldnt figure out the formula
}

float raymarch(float rox, float roy, float roz, float rdx, float rdy, float rdz) {
    float d = 0;
    int i;
    for(i = 0; i < 100; i++) {
        float px = rox + rdx*d;
        float py = roy + rdy*d;
        float pz = roz + rdz*d;
        float s = scene(px, py, pz);
        if(s < 0.01) {
            // hit something, return distance
            // TODO: do proper lighting here, just returning d for now
            return d;
        }
        d = d + s;
        if(d > 20) break;
    }
    return -1; // no hit
}

// ascii brightness thing i found online
char brightness(float d, float maxd) {
    if(d < 0) return ' ';
    float t = 1.0 - (d / maxd); // closer = brighter
    if(t < 0) t = 0;
    if(t > 1) t = 1;
    // probably a better way to do this
    char* chars = " .:-=+*#%@";
    return chars[(int)(t * 9)];
}

int main() {
    char buf[H][W+1];
    int x, y;
    
    printf("\033[2J"); // clear screen
    
    while(1) {
        for(y = 0; y < H; y++) {
            for(x = 0; x < W; x++) {
                float u = (2.0*x - W) / H;
                float v = (2.0*y - H) / H;
                
                float rdx = u, rdy = v, rdz = 1.5;
                normalize(&rdx, &rdy, &rdz);
                
                float d = raymarch(camx, camy, camz, rdx, rdy, rdz);
                buf[y][x] = brightness(d, 5.0);
            }
            buf[y][W] = '\0';
        }
        
        printf("\033[H"); // go to top (less flicker than clearing)
        for(y = 0; y < H; y++) {
            printf("%s\n", buf[y]);
        }
        
        usleep(50000); // ~20fps, 30fps made it flicker more for some reason
    }
    return 0;
}
