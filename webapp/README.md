This is a [Next.js](https://nextjs.org/) project bootstrapped with [`create-next-app`](https://github.com/vercel/next.js/tree/canary/packages/create-next-app).

## Getting Started

First, run the development server:

```bash
npm run dev
# or
yarn dev
# or
pnpm dev
# or
bun dev
```

Open [http://localhost:3000](http://localhost:3000) with your browser to see the result.

You can start editing the page by modifying `app/page.tsx`. The page auto-updates as you edit the file.

This project uses [`next/font`](https://nextjs.org/docs/basic-features/font-optimization) to automatically optimize and load Inter, a custom Google Font.

## Creating a Static HTML Export

To create a static HTML version of this site:

1. **Build the static export:**
   ```bash
   npm run export
   ```

2. **The static files will be generated in the `out/` directory.**

3. **To serve the static files locally for testing:**
   ```bash
   npx serve out
   ```
   Or use any static file server.

### Important Notes for Static Export:

- **API Routes**: Static export doesn't support Next.js API routes. Your API calls to `/api/*` will need to point directly to `http://192.168.4.1/api/*` in production.

- **Development vs Production**:
  - For development with API proxy: Uncomment the `rewrites()` function in `next.config.mjs`
  - For static export: Keep the `rewrites()` commented out and use `output: 'export'`

- **Content Files**: Make sure your `/content/*` files are either:
  - Placed in the `public/content/` directory, OR
  - Served from the device at `http://192.168.4.1/content/*`

- **Images**: All images are set to `unoptimized: true` to work with static export.

## Learn More

To learn more about Next.js, take a look at the following resources:

- [Next.js Documentation](https://nextjs.org/docs) - learn about Next.js features and API.
- [Learn Next.js](https://nextjs.org/learn) - an interactive Next.js tutorial.

You can check out [the Next.js GitHub repository](https://github.com/vercel/next.js/) - your feedback and contributions are welcome!

## Deploy on Vercel

The easiest way to deploy your Next.js app is to use the [Vercel Platform](https://vercel.com/new?utm_medium=default-template&filter=next.js&utm_source=create-next-app&utm_campaign=create-next-app-readme) from the creators of Next.js.

Check out our [Next.js deployment documentation](https://nextjs.org/docs/deployment) for more details.
