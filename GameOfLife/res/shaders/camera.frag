uniform sampler2D texture;
uniform vec2 offset;
uniform int zoom;

void main()
{
	vec2 UV = (gl_TexCoord[0].xy - vec2(0.5, 0.5)) / (float)(1 << zoom) + offset + vec2(0.5, 0.5);

	gl_FragColor = texture2D(texture, UV);
}