uniform float time;
varying vec4 Pos;
varying vec4 Vert;

void main()
{
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
 
  //gl_Position[0] = gl_Position[0]/10 * gl_ModelViewMatrix;
  gl_Position[0] = sin(5 * time + gl_Position[1] * 10)/20+gl_Position[0];
//glEnable(GL_LUCK_BIT_8_SPACEINVADERS); 
  vec3 transformedNormal = gl_NormalMatrix * gl_Normal;
  
  
  Pos = gl_Position;
  Vert = gl_Vertex;
  float timez = -3 + time;
  timez /= 10.0;
  if (Pos.x > timez)
	gl_Position.y += 2;

  gl_FrontColor = transformedNormal.zzzz;
  //gl_FrontColor = gl_Position.xyzx;
}
