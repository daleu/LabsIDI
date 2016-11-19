#version 330 core

out vec4 FragColor;

void main() {
    if (int(gl_FragCoord.y)%10>=0 && int(gl_FragCoord.y)%10<5)discard;
    if(gl_FragCoord.x < 347){
      if(gl_FragCoord.y < 400)FragColor = vec4(1, 1, 0, 1);
      else FragColor = vec4(1, 0, 0, 1);
    } else {
      if(gl_FragCoord.y < 400)FragColor = vec4(0, 0, 1, 1);
      else FragColor = vec4(0, 1, 0, 1);
    }
}

