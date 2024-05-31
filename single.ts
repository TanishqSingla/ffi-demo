import { suffix, dlopen } from "bun:ffi";
import fs from 'fs'

const libpath = `libsingle.${suffix}`;

const { symbols } = dlopen(libpath, {
	parse_metadata: {
		args: ['cstring'],
		returns: 'cstring'
	},
});

const files = [];
for(let i = 0; i < 1000; i++) {
	files.push(...fs.readdirSync("posts/"));
}
console.log(files.map(file => JSON.parse(String(symbols.parse_metadata(Buffer.from("posts/" + file + "\0"))))))
