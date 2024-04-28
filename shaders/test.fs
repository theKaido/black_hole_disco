 #version 330
in vec2 vsoTexCoord;
in vec4 vsoPos;
uniform float fond;
out vec4 fragColor;

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}



float noise(vec2 P) {
  vec2 Pi = ONE * floor(P) + ONEHALF;
  vec2 Pf = fract(P);
  vec2 grad00 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);
  vec2 grad10 = texture(permTexture, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));
  vec2 grad01 = texture(permTexture, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));
  vec2 grad11 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));
  return mix(n_x.x, n_x.y, fade(Pf.y));
}

float snoise(vec2 P) {
  float F2 = 0.366025403784;
  float G2 = 0.211324865405;
  float s = (P.x + P.y) * F2;
  vec2 Pi = floor(P + s);
  float t = (Pi.x + Pi.y) * G2;
  vec2 P0 = Pi - t;
  Pi = Pi * ONE + ONEHALF;
  vec2 Pf0 = P - P0;
  vec2 o1;
  if (Pf0.x > Pf0.y) o1 = vec2(1.0, 0.0);
  else o1 = vec2(0.0, 1.0);
  vec2 grad0 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float t0 = 0.5 - dot(Pf0, Pf0);
  float n0 = max(0.0, t0 * t0 * dot(grad0, Pf0));
  vec2 Pf1 = Pf0 - o1 + G2;
  vec2 grad1 = texture(permTexture, Pi + o1 * ONE).rg * 4.0 - 1.0;
  float t1 = 0.5 - dot(Pf1, Pf1);
  float n1 = max(0.0, t1 * t1 * dot(grad1, Pf1));
  vec2 Pf2 = Pf0 - vec2(1.0 - 2.0 * G2);
  vec2 grad2 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float t2 = 0.5 - dot(Pf2, Pf2);
  float n2 = max(0.0, t2 * t2 * dot(grad2, Pf2));
  return 10.0 * (n0 + n1 + n2);
}

void main() {
    float frequency = 35.0;
    
    // Calcul des bruits Perlin et Simplex avec une fréquence et zoom
    float perlin_noise = noise(vsoTexCoord * fond * frequency);
    float simplex_noise = snoise(vsoTexCoord * fond );
    
    // Calcul des normales perturbées
    vec3 normal = normalize(vec3(perlin_noise, simplex_noise, 1.0));
    
    
    vec3 bumped_position = vsoPos.xyz + normal * 0.1;
    
    // Calcul de la couleur en utilisant les bruits Perlin et Simplex
    vec3 color = vec3(perlin_noise + simplex_noise*30, perlin_noise + simplex_noise*30.0, perlin_noise + simplex_noise*30.0); 
    
    fragColor = vec4(color, bumped_position);
}

