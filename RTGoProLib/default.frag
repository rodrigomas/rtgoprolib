uniform sampler2D renderedTexture;
uniform float w;
uniform float h;

uniform  float FadeRight;
uniform  float FadeLeft;
uniform  float FadeRightPower;
uniform  float FadeLeftPower;
uniform  float Gamma;
uniform  float VerticalFade;

uniform  float OffsetTX;
uniform  float OffsetTY;

uniform float RotateImg;
uniform float FlipRGB;

float remaper_left(float val)
{
	//return 1.0 - smoothstep(0, 1, val / FadeLeft);
	return val / (FadeLeft);
}

float remaper_right(float val)
{
	//return 1.0 - smoothstep(0, 1, val / FadeRight);
	return val / (1.0 - FadeRight);
}

float border_factor(float t)
{
	float f = abs(t - 0.5) * 2.0;

	return (f > VerticalFade) ? ( 1.0 - smoothstep(0.0, 1.0 - VerticalFade, (f - VerticalFade))) : 1.0;
}

float bezier(float t, float power)
{
	return pow(1.0 - t, power);
	//return pow(1.0 - t, 1);
}

 void main()
 {
	vec2 tex = gl_TexCoord[0].st / 2.0;
	
	if(RotateImg < -89.0)
		tex = tex.ts;
	else if(RotateImg > 89.0)
		tex = vec2(tex.t, 0.5 - tex.s);
	
	tex += vec2(OffsetTX, OffsetTY);

	vec4 color = texture2D(renderedTexture, tex) * gl_Color;

	if(FlipRGB > 0.0)
		color.rgb = color.bgr;

	float alpha = (gl_TexCoord[0].x < FadeLeft) ? bezier(remaper_left(FadeLeft - gl_TexCoord[0].x), FadeLeftPower) : 1.0;

	alpha = (gl_TexCoord[0].x > FadeRight) ? bezier(remaper_right(gl_TexCoord[0].x - FadeRight), FadeRightPower) : alpha;

	alpha = alpha * border_factor(gl_TexCoord[0].y);

	color.rgb = color.rgb * Gamma;

	color.a *= alpha;

	gl_FragColor = color;
 }
