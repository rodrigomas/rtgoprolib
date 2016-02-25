uniform float dx;
uniform float dy;
uniform float w;
uniform float h;

uniform float bend_ur;
uniform float bend_dr;
uniform float bend_dl;
uniform float bend_ul;

uniform float bend_left;
uniform float bend_right;

uniform float FlipY;

uniform float UsePolar;

const float PI = 3.1415926535;

vec2 Distort(vec2 p, float d)
{
    vec2 v = p.xy;
    
	if(UsePolar > 0.0)
	{
		float radius = length(v);
		if (radius > 0.0)
		{
		  float theta = atan(v.y,v.x);
      
		  radius = pow(radius, d * 0.01 + 1.0);
  
		  v.x = radius * cos(theta);
		  v.y = radius * sin(theta);
		  p.xy = v.xy;
		}
	
		return 0.5 * (p + 1.0);
	} else 
	{
		v.y = v.y + (v.x * v.x) * d * v.y;
	}	

	if (p.x < 0.0)
	{
		v.x = v.x + bend_left * (p.y * p.y) * v.x;
	} else 
	{
		v.x = v.x + bend_right * (p.y * p.y) * v.x;
	}	

	return 0.5 * (v + 1.0);
}

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

	if(FlipY > 0.0)
		gl_TexCoord[0].t = 1.0 - gl_TexCoord[0].t;
	
	vec2 tex = 2.0 * (gl_MultiTexCoord0.xy - 0.5);
	vec4 vt = gl_Vertex;

	if( tex.x > 0.0 )
	{
		if( tex.y > 0.0 )  //Reversed
		{
			vt.xy = Distort(tex, bend_dr);	
		}
		else
		{
			vt.xy = Distort(tex, bend_ur);
		}
		
	} 
	else
	{
		if( tex.y > 0.0 )
		{
			vt.xy = Distort(tex, bend_dl);
		} else 
		{
			vt.xy = Distort(tex, bend_ul);
		}		
	}	

	vt.x = vt.x * w + dx;
	vt.y = vt.y * h + dy;

	gl_FrontColor = gl_Color;

	gl_Position = gl_ModelViewProjectionMatrix * vt;
}
