#version 330 core

in vec3 fcolor;
uniform float blneg;
out vec4 FragColor;

void main()
{	
   if(blneg==1.0) FragColor = vec4(fcolor,1);
   else {
    if(int(gl_FragCoord.y)%20>=0 && int(gl_FragCoord.y)%20<10)FragColor = vec4(1,1,1,1);
    else FragColor = vec4(0,0,0,1);
   }
}
